#include "../firmware/monitoring_logic.h"
#include <cassert>
#include <iostream>
#include <limits>
using namespace monitoring;
Sample normal() { return {28, 55, 230, 1, true, true}; }
int main() {
  { Monitor m; auto r = m.update(normal()); assert(r.level == NORMAL && r.activeAnomalies == 0); }
  // Inclusive threshold edges and values just below/inside warning.
  const float warnings[] = {35, 75, 210, 3};
  const float criticals[] = {40, 85, 200, 5};
  for (unsigned i = 0; i < 4; ++i) {
    for (unsigned severity = 0; severity < 2; ++severity) {
      Monitor m; Sample s = normal();
      float* fields[] = {&s.temperature, &s.humidity, &s.voltage, &s.current};
      *fields[i] = severity ? criticals[i] : warnings[i];
      auto r = m.update(s);
      assert(r.level == (severity ? CRITICAL : WARNING));
      assert(r.activeAnomalies == 1);
    }
  }
  for (float v : {250.0f, 260.0f}) {
    Monitor m; auto s = normal(); s.voltage = v;
    assert(m.update(s).level == (v == 250 ? WARNING : CRITICAL));
  }
  { Monitor m; auto s = normal(); s.temperature=34.9f; s.humidity=74.9f;
    s.current=2.9f; s.voltage=210.1f; assert(m.update(s).level == NORMAL); }
  { Monitor m; auto s=normal(); s.temperature=35; s.humidity=75;
    auto r=m.update(s); assert(r.level==CRITICAL && r.activeAnomalies==2);
    assert(r.cause.find("Temperature")!=std::string::npos && r.cause.find("Humidity")!=std::string::npos); }
  { Monitor m; auto s=normal(); s.temperature=40; s.humidity=85; s.voltage=260; s.current=5;
    assert(m.update(s).activeAnomalies==4); }
  // Four consecutive increasing samples, not just an endpoint difference.
  { Monitor m; Result r; for (float t : {28.f,28.5f,29.f,29.5f}) {
      auto s=normal(); s.temperature=t; r=m.update(s);
      if (t<29.5f) assert(r.level==NORMAL);
    } assert(r.level==WARNING && r.activeAnomalies==1); }
  { Monitor m; Result r; for (float t : {28.f,29.f,28.5f,30.f}) {
      auto s=normal(); s.temperature=t; r=m.update(s);
    } assert(r.level==NORMAL); }
  { Monitor m; Result r; for (float t : {28.f,28.f,29.f,30.f}) {
      auto s=normal(); s.temperature=t; r=m.update(s);
    } assert(r.level==NORMAL); }
  { Monitor m; Result r; for (int i=0;i<4;++i) {
      auto s=normal(); s.temperature=28+i; s.current=1+i*0.2f; r=m.update(s);
    } assert(r.level==CRITICAL && r.activeAnomalies==2);
    assert(r.trend.find("CURRENT RISING")!=std::string::npos); }
  { Monitor m; Result r; for (float t : {35.f,36.f,37.f,38.f}) {
      auto s=normal(); s.temperature=t; r=m.update(s);
    } assert(r.level==WARNING && r.activeAnomalies==1); }
  for (int direction : {-1,1}) {
    Monitor m; Result r; for (int i=0;i<4;++i) {
      auto s=normal(); s.voltage=230+direction*i*2; r=m.update(s);
    } assert(r.level==WARNING && r.activeAnomalies==1);
  }
  { Monitor m; Result r; for (float h : {55.f,57.f,59.f,61.f}) {
      auto s=normal(); s.humidity=h; r=m.update(s);
    } assert(r.level==WARNING); }
  // Fault priority preserves anomalies from the healthy sensor; both faults reported.
  { Monitor m; auto s=normal(); s.dhtValid=false; s.current=5;
    auto r=m.update(s); assert(r.level==SYSTEM_FAULT && r.activeAnomalies==1);
    s.pzemValid=false; r=m.update(s); assert(r.activeAnomalies==0);
    assert(r.cause.find("DHT11")!=std::string::npos && r.cause.find("PZEM")!=std::string::npos); }
  { Monitor m; auto s=normal(); s.temperature=std::numeric_limits<float>::infinity();
    assert(m.update(s).level==SYSTEM_FAULT); }
  { Monitor m; auto s=normal(); s.current=std::numeric_limits<float>::quiet_NaN();
    assert(m.update(s).level==SYSTEM_FAULT); }
  // Invalid readings break the affected history even when an old value is retained.
  { Monitor m; auto s=normal(); s.temperature=28; m.update(s);
    s.temperature=29; m.update(s); s.dhtValid=false; m.update(s);
    s.dhtValid=true; s.temperature=30; assert(m.update(s).trend=="STABLE");
    s.temperature=31; assert(m.update(s).trend=="STABLE"); }
  // Recovery requires three consecutive NORMAL results and resets on new abnormality.
  { Monitor m; auto s=normal(); s.temperature=40; m.update(s);
    auto r=m.update(normal()); assert(r.level==CRITICAL && r.recoveryCount==1 && r.activeAnomalies==0);
    assert(r.cause.find("Recovery pending")!=std::string::npos);
    assert(m.update(normal()).recoveryCount==2);
    assert(m.update(normal()).level==NORMAL);
    m.update(s); m.update(normal()); m.update(s);
    assert(m.update(normal()).recoveryCount==1);
  }
  { Events e; assert(!e.due(NORMAL,0,true)); assert(!e.due(WARNING,0,false));
    assert(e.due(WARNING,0,true)); e.markSent(WARNING,0);
    assert(!e.due(WARNING,2000,true)); assert(e.due(CRITICAL,2000,true)); e.markSent(CRITICAL,2000);
    assert(!e.due(WARNING,3000,true)); assert(e.due(WARNING,60000,true));
    assert(e.due(NORMAL,60000,true)); e.markSent(NORMAL,60000);
    assert(!e.due(NORMAL,62000,true));
  }
  { Events e; e.markSent(WARNING,UINT32_MAX-1000); e.markSent(CRITICAL,UINT32_MAX-500);
    assert(!e.due(WARNING,1000,true)); assert(e.due(WARNING,60000,true)); }
  std::cout << "PASS: threshold, trend, fault, recovery, multi-anomaly and event policy\n";
}
