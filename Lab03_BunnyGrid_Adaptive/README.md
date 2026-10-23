# Lab03 — Stanford Bunny Adaptive Grid / Rotation-Safe Fixed Scale

**Continuation of Lab01_BunnyGrid_BruteForce and Lab02_BunnyGrid_Instancing**

C++17 · OpenGL 3.3 Core · GLSL 330 · GLFW · GLAD1 · CMake · Visual Studio

## Objective

The user can render ANY integer number of Stanford Bunnies from **1 to 10,000**.
Every Bunny is inside the visible 800×600 window in an adaptive orthographic grid.
Every Bunny can independently rotate around X/Y/Z without colliding with a neighbor.
A **single uniform scale is computed when the layout is initialized or changed**, not every frame.
There is **NO frustum/occlusion culling** in Lab03: all instances are deliberately visible.

The Bunny is the **same 2,503-vertex / 4,968-triangle** dataset used in Lab01 and Lab02; `include/bunny_data.h` is included. Its vertex format is x,y,z,r,g,b with indexed GL_TRIANGLES.

## Directory layout

```text
Lab03_BunnyGrid_Adaptive/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── bunny_data.h            # Same 2,503 vertex Stanford Bunny as Lab01 / Lab02
│   ├── Lab03Math.h             # Geometry/grid types and reference functions
│   └── Lab03Student.h          # Functions to implement for exercises
├── src/
│   ├── main.cpp                # Runnable instanced demo with custom N input
│   ├── Lab03Math.cpp           # Working reference math used by default
│   └── Lab03Student.cpp        # TODO 1–4 (student implementation)
├── shaders/
│   ├── bunny.vert              # gl_InstanceID / XYZ animation
│   └── bunny.frag
├── tests/
│   ├── test_layout.cpp         # Reproducible geometry/layout tests
│   └── test_student.cpp        # Optional tests for YOUR TODO answers
├── third_party/README.md
└── results/README.md
```

### IMPORTANT: Two intentional modes

**Default demonstration mode** uses the reference mathematics. **Student mode** uses the four exercise implementations in `src/Lab03Student.cpp`. Those functions have been implemented in this submitted version and are checked against the reference by unit tests. The original exercise prompts remain in comments for learning purposes.

Keep reference math unchanged when experimenting with student algorithms. The student file preserves the exercise explanations and submitted solutions.

## Visual Studio 2022 / Windows CMake build

1. Extract the ZIP into the same `ComputerGraphicsLabs` parent directory as Lab01 and Lab02.
2. Open this lab folder in Visual Studio (**Open > Folder**) or use a terminal.
3. From a VS Developer PowerShell/CMD:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
.\build\Release\Lab03_BunnyGrid_Adaptive.exe
```

For the Visual Studio CMake Open Folder workflow, use `cmake -S . -B build` (no `-A` for Ninja generators). If OpenGL/GLFW/GLAD are already provided by your existing Lab02 setup, feel free to reuse those local dependencies.

GLFW and GLAD1 may be fetched from their upstream repositories on first configuration (Internet needed). If you prefer building offline, follow `third_party/README.md` to supply your working GLFW + GLAD from Lab02.

## Controls

- At startup, the console asks: `How many Bunnies (1..10000)? [100]`.
- **While window is open:** type any digits (`37`, `256`, `7500`, etc.) and press **Enter** to apply the new count.
- **Backspace** edits pending count.
- **R** toggle XYZ rotation (time resumes where paused).
- **B** toggle projected bounding-sphere safety circles (only drawn for N <= 1000 for visibility).
- **W** toggle wireframe.
- **V** toggle VSync (default OFF, as in Lab01/02).
- **Esc** exit.
- The title bar displays N, grid columns/rows, FPS, and pending input.
- Resize the window to recalculate the fixed scale and positions once for the *new viewport size*. Rotation alone never changes the scale.

## Mathematics

Take the AABB midpoint `c` as the rotation pivot.

```
AABB.min = componentwise minimum over all mesh vertices
AABB.max = componentwise maximum over all mesh vertices
center = (AABB.min + AABB.max) / 2
r = max_vertex ||position - center||
```

Let N be Bunny count, W/H the ORTHOGRAPHIC WORLD-SPACE viewport dimensions (W/H has the same aspect ratio as framebuffer pixels).

```
columns = min(N, ceil(sqrt(N * W/H)))
rows    = ceil(N / columns)
cellW   = W / columns
cellH   = H / rows
scale   = 0.85 * min(cellW, cellH) / (2*r)
```

Center of Bunny #i:

```
col = i % columns
row = i / columns
x = -W/2 + (col+0.5)*cellW
y =  H/2 - (row+0.5)*cellH
```

The transformation is **translation(cell center) × XYZ rotation × fixed scale × translation(-mesh center)**.
The Bunny remains in the enclosed sphere during any rigid rotation, hence a sphere with diameter <= 85% of the smallest cell dimension guarantees a gap to every neighboring Bunny.

When N=1, the cell *is* the whole viewport; when N=10,000 each instance may become tiny. This is correct; no zoom or culling is introduced.

## Implemented exercises (100 points)

Open **`src/Lab03Student.cpp`**, review the completed functions in order:

1. **TODO 1, BoundingSphere (20 pts):** AABB center and maximum vertex distance. Justify invariance under rotation.
2. **TODO 2, GridLayout (25 pts):** any integer 1..10000, correct C/R, aspect-aware cells.
3. **TODO 3, FixedScale (25 pts):** 15% safety gap, constant scale through animation.
4. **TODO 4, InstanceTransforms (30 pts):** calculate cell positions and different XYZ rotations. Ensure CPU transformations agree conceptually with `gl_InstanceID` in the shader.

BONUS: TODO 5 in `shaders/bunny.vert`, explain why GPU instancing does not require 10,000 CPU draw calls.

Run the reference and student tests:

```powershell
cmake --build build --config Release --target lab03_student_tests
ctest --test-dir build -C Release --output-on-failure
.\build\Release\lab03_student_tests.exe
```

To run the student algorithms in the actual rendering program:

```powershell
cmake -S . -B build-student -A x64 -DLAB03_USE_STUDENT_MATH=ON
cmake --build build-student --config Release
.\build-student\Release\Lab03_BunnyGrid_Adaptive.exe
```

> The reference and student algorithms are separate, and the selected mode is explicit at CMake configuration time.

## Performance caution

The grid in Lab03 is different from the fixed 10-column layout of Lab01/02, so FPS is **not a controlled comparison** to their historical benchmark numbers. A 10,000 Bunny scene still processes 10,000 instances and 149,040,000 indexed vertices' worth of triangle indices, even when bunnies are individually only a few pixels tall. Actual shader executions and rasterizer behavior depend on GPU optimizations and clipping. Benchmark identical conditions if comparing brute force against instancing.

## Next labs

Lab04: Lighting and shading foundations. Lab05: Metallic Bunny (view-dependent specular highlights, shininess and optional PBR). Future: shadows, frustum culling and LOD.

## GPU comparison experiment (planned; no measurements yet)

This repository is ready for a future controlled comparison of the **integrated GPU** and **NVIDIA RTX 4050** in the same laptop. Results have **not** been collected or verified. Follow `results/BENCHMARK_PLAN.md`; record the actual renderer name using the existing `GL_RENDERER` console output. Do not treat the window-title FPS as a GPU-only timing measurement.

## GitHub repository notes

Commit the source, shaders, tests, CMake configuration, documentation and the bundled mesh data. Do not commit build directories, generated executables or fabricated benchmark results. The `.github/workflows/cpp-tests.yml` workflow tests pure C++ layout algorithms without needing an OpenGL display.
