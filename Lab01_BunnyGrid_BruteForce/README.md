# Lab01 — Stanford Bunny Grid: Repeated Draw Calls

## Overview

This lab investigates the performance of rendering many copies of the
Stanford Bunny using repeated `glDrawElements()` calls.

The Stanford Bunny mesh is uploaded to GPU memory once using a Vertex Buffer
Object (VBO) and an Element Buffer Object (EBO). The same geometry is then
rendered repeatedly at different positions.

For every bunny, the CPU updates its position and issues a separate
`glDrawElements()` call.

This implementation provides a baseline for later experiments with GPU
instancing, visibility culling, spatial data structures, and Level of Detail
(LOD).

---

## Objectives

The main objectives of this lab are:

- Render multiple copies of the same Stanford Bunny mesh.
- Understand the cost of issuing many draw calls from the CPU.
- Observe how OpenGL clips geometry outside the visible region.
- Measure FPS and frame time with VSync disabled.
- Compare performance for increasing numbers of objects.
- Establish a baseline for a later GPU-instancing experiment.

---

## Rendering Pipeline

The bunny geometry is uploaded to GPU memory once:

```text
CPU
 │
 │ glBufferData()
 ▼
GPU
 ├── VBO → vertex data
 └── EBO → triangle indices
```

The mesh is not uploaded again for every bunny.

Instead, every frame the program reuses the same VAO/VBO/EBO and renders the
mesh at different positions.

Conceptually:

```text
for every bunny:
    calculate position
    update uOffset
    update uScale
    glDrawElements(...)
```

Therefore, if the scene contains `N` bunnies, approximately `N` separate draw
calls are issued per frame.

For example:

```text
1 bunny       →      1 draw call / frame
100 bunnies   →    100 draw calls / frame
1,000 bunnies →  1,000 draw calls / frame
10,000 bunnies→ 10,000 draw calls / frame
```

This is intentionally a brute-force rendering approach.

---

## Benchmark Modes

When the application starts, the user can select:

```text
=============================
      BUNNY BENCHMARK
=============================

How many bunnies would you like to render?

1. 1 bunny
2. 100 bunnies
3. 1000 bunnies
4. 10000 bunnies
```

These workloads allow the effect of increasing the number of draw calls to be
observed.

---

## Bunny Grid

Each bunny receives a different 2D offset through the `uOffset` shader uniform.

The current implementation uses a grid with 10 columns:

```cpp
int columns = 10;
```

The position of each bunny is calculated from its row and column:

```cpp
int row = i / columns;
int col = i % columns;

float x = -0.9f + col * 0.20f;
float y =  0.9f - row * 0.20f;
```

The position is then sent to the shader:

```cpp
glUniform2f(offsetLocation, x, y);
```

and the bunny is rendered:

```cpp
glDrawElements(
    GL_TRIANGLES,
    (GLsizei)BunnyMesh::indexCount,
    GL_UNSIGNED_INT,
    nullptr
);
```

---

## Clipping Observation

Some bunnies may be partially or completely outside the visible area.

This provides a useful demonstration of the OpenGL graphics pipeline.

After the vertex shader produces `gl_Position`, primitives are processed
against the clip volume before rasterization. Geometry outside the visible
region can therefore be clipped automatically by the graphics pipeline.

Conceptually:

```text
Vertex Data
    ↓
Vertex Shader
    ↓
Clip-Space Position
    ↓
Clipping
    ↓
Rasterization
    ↓
Fragment Shader
    ↓
Framebuffer
```

It is important to distinguish **clipping** from **visibility culling**.

In this lab, the CPU still issues a draw call for every bunny. A bunny that is
outside the visible region may later be rejected by the graphics pipeline, but
the draw call has already been submitted.

A later experiment can perform visibility tests before rendering objects so
that unnecessary work is avoided earlier.

---

## Benchmark Configuration

Vertical synchronization is disabled:

```cpp
glfwSwapInterval(0);
```

This prevents the benchmark from being artificially limited by the monitor
refresh rate.

The program also prints the active OpenGL implementation:

```text
OpenGL Vendor
OpenGL Renderer
OpenGL Version
```

This makes it possible to verify which GPU is executing the benchmark.

---

## Performance Measurement

The program counts rendered frames and calculates FPS approximately once per
second.

Frame time is calculated from:

```text
frame time (ms) = 1000 / FPS
```

Example console output:

```text
FPS: 460.0 | Frame time: 2.17 ms
```

FPS indicates how many frames are produced per second, while frame time is
usually more useful when comparing rendering performance because it shows the
time required to produce one frame.

---

## Geometry Workload

The Stanford Bunny used in this experiment contains approximately 14,000
triangles.

Therefore, a scene containing 10,000 bunny instances represents approximately:

```text
14,000 × 10,000
≈ 140,000,000 triangles
```

submitted as repeated instances of the mesh per frame.

This value describes the theoretical submitted geometry workload. It should
not be interpreted as meaning that every triangle necessarily reaches full
rasterization, since clipping and other GPU pipeline operations may reject
geometry.

---

## Important Limitation

The current grid uses a fixed number of 10 columns and a fixed bunny scale.

As the number of bunnies becomes very large, many objects extend beyond the
visible region.

Therefore, results from this version should be interpreted as measurements of
the original repeated-draw-call experiment rather than as a benchmark in which
all 10,000 objects are simultaneously visible.

A future controlled comparison can use a dynamically sized grid so that the
same scene configuration is used for both repeated draw calls and GPU
instancing.

---

## Why This Lab Matters

The mesh itself is already stored in GPU memory.

The main inefficiency investigated here is that the CPU repeatedly communicates
rendering commands to the graphics API:

```text
CPU
 │
 ├── Draw Bunny 1
 ├── Draw Bunny 2
 ├── Draw Bunny 3
 ├── ...
 └── Draw Bunny N
        ↓
       GPU
```

With thousands of objects, draw-call and driver overhead can become an
important part of rendering performance.

This motivates the next experiment.

---

## Next Lab — GPU Instancing

`Lab02` will render many copies of the same Stanford Bunny using GPU instancing.

Instead of:

```text
10,000 bunnies
        ↓
10,000 glDrawElements() calls
```

the goal will be:

```text
10,000 bunny instances
        ↓
instance data
        ↓
1 glDrawElementsInstanced() call
```

The mesh will still be stored once in GPU memory, but per-instance information
such as position will be supplied separately.

The comparison between Lab01 and Lab02 will investigate how reducing CPU-side
draw-call overhead affects rendering performance.

---

## Results

Detailed benchmark observations and measurements are stored in:

```text
results/Lab01_Performance_Report.pdf
```

The report documents the tested hardware, workloads, FPS/frame-time
measurements, observations, and limitations of the experiment.

---

## Technologies

- C++17
- OpenGL 3.3 Core Profile
- GLSL 3.30
- GLFW
- GLAD
- CMake
- Visual Studio
- Stanford Bunny mesh