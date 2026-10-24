# Benchmark protocol — Lab 03 and Lab 04

**Status: planned; measurements not yet collected.**

## Shared experimental conditions

- Laptop on AC power, stable power profile, comparable temperatures and background load.
- Build **Release**, VSync OFF, identical window resolution, Bunny counts and rotation state.
- Verify the actual OpenGL renderer using `glGetString(GL_RENDERER)` before each run. Windows GPU preferences are not proof that a specific GPU is active.
- Disable bounding-sphere outlines and wireframe. Allow warm-up; sample a fixed duration (suggested 30 seconds), three repetitions.
- Record measured FPS and frame times; frame-time percentiles require per-frame logging, which the current app does not provide.
- `1000 / observed FPS` is an approximate frame duration, not an isolated GPU timing measurement.

## Lab 03: adaptive grid scaling

Test Bunny counts `1,10,25,50,100,200,500,1000` where practical. Compare integrated AMD and NVIDIA RTX 4050 under identical settings. 4,968 triangles per Bunny; total triangles per frame = Bunny count × 4,968.

CSV template: `lab03_measurements_TEMPLATE.csv`.

## Lab 04: fragment shading cost

At each selected Bunny count, test the three modes independently: `solid`, `checkerboard`, `fireflies`. Record checker frequency, pulse speed and glow intensity so runs are reproducible. Keep all other settings fixed.

CSV template: `lab04_measurements_TEMPLATE.csv`.

## Analysis after collection

Plot observed FPS and approximate frame time against Bunny count; for Lab 04 compare shader modes on the same GPU. Discuss fragment workload, screen coverage, aliasing and CPU/GPU bottlenecks without claiming that these have been isolated by FPS alone. GPU timer queries can be added later for more precise GPU timings.
