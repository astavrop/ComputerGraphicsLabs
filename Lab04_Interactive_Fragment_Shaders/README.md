# Lab 04 — Interactive Fragment Shaders 🐰

**C++17 · OpenGL 3.3 Core · GLSL 330 · GLFW · GLAD1 · Dear ImGui · CMake**

An interactive Stanford Bunny graphics lab extending the adaptive grid from Lab 03. The included mesh has **2,503 vertices and 4,968 triangles per Bunny**. Every Bunny is rendered; no LOD or culling is implemented.

## Features

- Adaptive orthographic grid and animated XYZ rotations using instanced rendering.
- Live Bunny count adjustment (slider and numeric input) through Dear ImGui.
- Three fragment shader modes: **Solid Color**, **Checkerboard**, and **Fireflies**.
- Solid color picker; two checkerboard color pickers and frequency; firefly dark/bright colors, pulse speed and glow intensity.
- On-screen geometry statistics; observed FPS and frame time in the window title.
- Reusable Lab 03 reference geometry math and student exercises with automated C++ tests.

> **Benchmark status:** Lab 03 and Lab 04 measurements are **PENDING**. No GPU performance conclusions are claimed. See [`results/BENCHMARK_PLAN.md`](results/BENCHMARK_PLAN.md) and [`results/README.md`](results/README.md).

## Build on Windows (Visual Studio 2022)

Install Visual Studio with the **Desktop development with C++** workload, CMake, and a graphics driver supporting OpenGL 3.3. An internet connection is needed the first time CMake fetches dependencies (GLFW, GLAD, Dear ImGui), unless provided locally.

Open a **Developer PowerShell for VS 2022** in the repository root:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
.\build\Release\Lab04_Interactive_Fragment_Shaders.exe
```

Alternatively, use Visual Studio **File → Open → Folder**, choose an **x64-Release / MSVC** configuration, and build the target `Lab04_Interactive_Fragment_Shaders`. Do not use a MinGW configuration unless you have configured a working MinGW C/C++ toolchain.

The app reads shader files from the repository `shaders/` directory via a compiled absolute source path, so moving only the `.exe` is **not** a portable distribution.

### Headless tests (no OpenGL required)

```bash
cmake -S . -B build-tests -DLAB03_BUILD_GRAPHICS=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-tests
ctest --test-dir build-tests --output-on-failure
```

The legacy `LAB03_*` CMake options and `Lab03Math` source names are retained intentionally because this lab builds on Lab 03.

## Controls

| Control | Purpose |
|---|---|
| Dear ImGui Bunny slider / number field | Change Bunny count live |
| Rendering mode selector | Solid Color / Checkerboard / Fireflies |
| Mode-specific sliders / color editors | Customize the active shader |
| R | Toggle rotation |
| B | Toggle bounding sphere outlines |
| W | Toggle wireframe |
| V | Toggle VSync |
| Esc | Exit |

Keyboard entry from Lab 03 also remains available, but use the ImGui controls for normal operation. See `src/main.cpp` for input handling details.

## Learning goals

1. Distinguish vertex and fragment shader responsibilities.
2. Send C++ GUI values to GLSL using uniforms.
3. Explore procedural color patterns and time-dependent effects.
4. Understand aliasing at high checkerboard frequencies; **procedural antialiasing is not implemented**.
5. Compare rendering workloads under reproducible conditions after benchmarks are collected.

**Note:** The Fireflies mode interpolates colors over time. It does not implement HDR bloom or a physically based lighting model.

## Repository structure

```text
include/       Stanford Bunny data and Lab 03 math headers
src/           C++ application, reference math and student solutions
shaders/       GLSL vertex and fragment shaders
tests/         C++ geometry tests
results/       Benchmark protocol and future results
.github/       Headless CI tests
third_party/   Instructions for optional local dependencies
```

## Benchmark roadmap

- **Lab 03:** adaptive grid performance vs Bunny count on integrated AMD and NVIDIA RTX 4050 GPUs.
- **Lab 04:** compare the three fragment modes at fixed Bunny counts, resolution and GPU settings.
- Collect real measurements in **Release**, VSync off, with repeated runs and renderer verification (`GL_RENDERER`).
- Publish measured CSV files and graphs **only after data collection**; see the result templates.

## Attribution

The Stanford Bunny mesh is bundled as `include/bunny_data.h`. GLFW, GLAD and Dear ImGui are external dependencies retrieved by CMake or supplied locally. Consult their upstream licenses when redistributing binaries or dependencies.
