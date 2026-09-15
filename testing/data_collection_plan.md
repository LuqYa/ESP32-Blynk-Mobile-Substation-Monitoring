# Data Collection Plan

Collect repeatable laboratory measurements using [test_procedure.md](test_procedure.md) and [thresholds.md](thresholds.md). Sample nominally every two seconds and repeat principal tests at least three times where practical.

Use [test_results_v0_v11.csv](test_results_v0_v11.csv). Record V0–V3 measurements, V4 condition, V5 cause, V6 action, observed V7/V8 colors, V9 trend text, V10 severity, V11 count, recovery progress, event code/time and phone notification delivery. A value of 1 at V7/V8 is LED brightness and does not prove health.

Capture stable baseline data before controlled threshold/trend tests. Record reference voltage/current where available:

`Error (%) = |PZEM value - reference value| / |reference value| × 100`

For a zero reference, report absolute error instead of dividing by zero. Do not describe comparisons as calibration without a proper calibration procedure.

Leave unavailable measurements blank and explain in remarks. Record no invented measurements. The older test_results.csv retains its original legacy columns for historical continuity; do not use its old pin meanings for this firmware. Optional Render advisory tests are separate from measurement/classification evidence.
