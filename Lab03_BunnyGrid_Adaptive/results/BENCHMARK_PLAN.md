# Lab03 — GPU benchmark plan (for the next session)

**Status:** protocol only; no measurements collected.

## Conditions

- Use the same laptop, display resolution, OS, CPU, power mode and application build for both GPU runs.
- Plug into AC power; keep cooling conditions as similar as possible.
- Select the integrated GPU and then the NVIDIA RTX 4050 using Windows Graphics settings for the executable.
- Confirm the **actual** selected GPU using `GL_RENDERER` printed on launch.
- Build **Release**; turn **VSync off (V)**, sphere outlines off (B) and wireframe off (W).
- Keep the window size and rotation setting identical.
- For each bunny count (1, 10, 25, 50, 100, 200, optionally 500 and 1000), allow a warm-up interval and measure over a fixed interval, e.g. 30 seconds; repeat three times.
- Record median FPS and frame-time distributions if frame logging is added. Watch for throttling and thermal drift.

## Data fields

`gpu_renderer,bunnies,vertices_per_bunny,triangles_per_bunny,total_triangles,resolution,vsync,rotation,run,mean_fps,mean_frame_ms,notes`

The included mesh contains **2,503 vertices and 4,968 triangles per Bunny**. With this version, bunny count changes the *total* geometry processed, not the mesh's per-Bunny polygon count. For high-/low-poly tests, prepare verified alternative meshes or a separate LOD variant later.

## Analysis

- Plot FPS vs number of bunnies for each GPU.
- Plot frame time vs total triangle count.
- Compare the 60 FPS and 30 FPS thresholds.
- Explain that the window-title FPS is an end-to-end observed frame rate, not an isolated GPU processing time.
- For a more controlled experiment, consider GPU timer queries and fixed-duration CSV logging in the next iteration.
- Do not claim measured FPS or speedups until real data exists.
