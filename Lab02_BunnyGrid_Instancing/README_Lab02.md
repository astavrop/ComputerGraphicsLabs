# Lab02 – Stanford Bunny Grid with GPU Instancing

## Overview

This laboratory is a controlled follow-up to `Lab01_BunnyGrid_BruteForce`.

Lab01 rendered multiple copies of the Stanford Bunny by issuing a separate `glDrawElements()` call for every object.

Lab02 replaces those repeated draw calls with GPU instancing:

```cpp
glDrawElementsInstanced(
    GL_TRIANGLES,
    (GLsizei)BunnyMesh::indexCount,
    GL_UNSIGNED_INT,
    nullptr,
    bunnyCount
);
```

The objective is to investigate whether reducing many draw calls to a single instanced draw call improves rendering performance.

The experiment was tested with:

- 1 bunny
- 100 bunnies
- 1,000 bunnies
- 10,000 bunnies

on both an integrated AMD Radeon GPU and an NVIDIA GeForce RTX 4050 Laptop GPU.

---

## Learning Objectives

This lab introduces and investigates:

- GPU instancing
- `glDrawElementsInstanced()`
- `gl_InstanceID`
- CPU-to-GPU draw submission
- draw-call overhead
- GPU workload scaling
- hardware-dependent performance
- experimental comparison of rendering strategies

A central question of the lab is:

> Does reducing thousands of draw calls to a single instanced draw call necessarily produce higher FPS?

The measurements show that the answer depends on the workload and hardware.

---

## Relationship to Lab01

Lab01 used a CPU loop similar to:

```cpp
for (int i = 0; i < bunnyCount; ++i)
{
    // Calculate grid position
    glUniform2f(offsetLocation, x, y);
    glUniform1f(scaleLocation, scale);

    glDrawElements(
        GL_TRIANGLES,
        BunnyMesh::indexCount,
        GL_UNSIGNED_INT,
        nullptr
    );
}
```

For 10,000 bunnies this means approximately:

```text
10,000 objects
      ↓
10,000 uniform updates
      ↓
10,000 glDrawElements() calls
```

Lab02 changes the rendering strategy to:

```text
10,000 objects
      ↓
1 glDrawElementsInstanced()
      ↓
10,000 instances processed by the GPU
```

The mesh itself is still stored only once.

---

## GPU Instance Identification

Each rendered instance receives an automatically generated instance number through GLSL:

```glsl
gl_InstanceID
```

For example:

```text
first bunny   → gl_InstanceID = 0
second bunny  → gl_InstanceID = 1
third bunny   → gl_InstanceID = 2
...
```

The vertex shader uses this value to determine the position of each bunny.

---

## GPU Grid Calculation

The current experiment uses a fixed grid with 10 columns.

Inside the vertex shader:

```glsl
int instanceID = gl_InstanceID;

int columns = 10;

int row = instanceID / columns;
int col = instanceID % columns;

float x = -0.9 + float(col) * 0.20;
float y =  0.9 - float(row) * 0.20;

vec2 instanceOffset = vec2(x, y);
```

For example, for `gl_InstanceID = 1`:

```text
row = 1 / 10 = 0
col = 1 % 10 = 1

x = -0.9 + 1 × 0.20 = -0.7
y =  0.9 - 0 × 0.20 =  0.9
```

so the instance offset is `(-0.7, 0.9)`.

All vertices belonging to that instance receive the same instance ID and therefore the same positional offset.

---

## Vertex Transformation

The instance position is applied in the vertex shader:

```glsl
gl_Position = vec4(
    p.x * uScale / uAspect + instanceOffset.x,
    p.y * uScale + instanceOffset.y,
    p.z * uScale,
    1.0
);
```

The current benchmark uses:

```text
uScale = 0.10
spacing = 0.20
columns = 10
```

These values are deliberately kept compatible with Lab01 so that the main experimental change is the rendering submission strategy.

---

## Important Note about `gl_InstanceID`

`gl_InstanceID` should not be interpreted as a CPU-style loop variable.

The vertex shader still executes for the vertices of every instance. However, all vertices belonging to one instance receive the same `gl_InstanceID`.

Conceptually:

```text
Instance 0
 ├─ vertex 0 → instanceID 0
 ├─ vertex 1 → instanceID 0
 └─ ...

Instance 1
 ├─ vertex 0 → instanceID 1
 ├─ vertex 1 → instanceID 1
 └─ ...
```

This allows the GPU to calculate a different transformation for each copy of the same mesh.

---

## Experimental Setup

| Component | Configuration |
|---|---|
| API | OpenGL 3.3 Core Profile |
| Language | C++ / GLSL 3.30 |
| Libraries | GLFW + GLAD |
| IDE | Visual Studio |
| Build | x64-Debug |
| Mesh | Stanford Bunny |
| VSync | Disabled |
| Integrated GPU | AMD Radeon(TM) Graphics |
| Discrete GPU | NVIDIA GeForce RTX 4050 Laptop GPU |
| NVIDIA Driver | 617.42 |

VSync is disabled using:

```cpp
glfwSwapInterval(0);
```

The program also prints the OpenGL renderer so that the GPU actually used by the application can be verified.

Example:

```text
OpenGL Vendor: NVIDIA Corporation
OpenGL Renderer: NVIDIA GeForce RTX 4050 Laptop GPU
```

---

## Lab02 Results

Approximate stabilized measurements:

| Bunnies | RTX 4050 FPS | RTX Frame Time | AMD iGPU FPS | AMD Frame Time |
|---:|---:|---:|---:|---:|
| 1 | ~2,250 | ~0.44 ms | ~4,700 | ~0.21 ms |
| 100 | ~2,050–2,100 | ~0.48 ms | ~1,400 | ~0.71 ms |
| 1,000 | ~727 | ~1.38 ms | ~355 | ~2.82 ms |
| 10,000 | ~85.7 | ~11.67 ms | ~81.5 | ~12.27 ms |

Short startup and transient FPS drops were excluded from the primary observations.

---

## Comparison with Lab01

### NVIDIA RTX 4050

| Bunnies | Lab01 FPS | Lab02 FPS | Lab01 Frame Time | Lab02 Frame Time |
|---:|---:|---:|---:|---:|
| 1 | ~2,250 | ~2,250 | ~0.44 ms | ~0.44 ms |
| 100 | ~708 | ~2,050–2,100 | ~1.41 ms | ~0.48 ms |
| 1,000 | ~710 | ~727 | ~1.41 ms | ~1.38 ms |
| 10,000 | ~85 | ~85.7 | ~11.7 ms | ~11.67 ms |

The strongest measured improvement occurred at 100 bunnies.

Frame time decreased approximately from `1.41 ms` to `0.48 ms`, corresponding to roughly a 2.9× reduction in frame time for that workload.

At 1,000 and 10,000 bunnies, however, the difference became much smaller.

---

## AMD Integrated GPU

The AMD measurements showed little performance improvement from instancing.

Lab01 and Lab02 produced broadly similar frame times across the tested workloads.

This is an important result because it demonstrates that an optimization does not necessarily produce the same performance effect on different hardware.

---

## Main Observation

The experiment does **not** support the simplistic conclusion:

> Instancing is always faster.

Instead, the measurements support a more precise statement:

> GPU instancing reduces CPU/driver draw-submission overhead, but its observable performance benefit depends on whether draw submission is an important bottleneck for the current workload and hardware.

At 100 bunnies on the RTX 4050, reducing repeated draw calls produced a large measured improvement.

At 10,000 bunnies, Lab01 and Lab02 were nearly identical despite the dramatic difference in draw-call count.

This suggests that another part of the rendering workload becomes dominant.

Determining the exact bottleneck would require profiling rather than FPS measurements alone.

---

## Fixed Grid Limitation

The current grid intentionally remains simple:

```text
columns = 10
spacing = 0.20
uScale = 0.10
```

For 100 bunnies this produces a visible 10 × 10 grid.

For 1,000 bunnies:

```text
10 columns × 100 rows
```

and for 10,000:

```text
10 columns × 1,000 rows
```

Most instances therefore extend below the visible viewport.

This is intentionally preserved from Lab01 so that the comparison focuses on draw submission rather than changing several variables simultaneously.

---

## Clipping Is Not Culling

Objects outside the visible viewport may eventually be rejected by the graphics pipeline, but they were still requested by the application.

GPU instancing does not automatically determine which objects are useful to render.

This leads naturally to a new question:

> Why submit thousands of objects if many of them cannot be seen?

That question motivates later experiments involving:

- bounding volumes
- frustum culling
- spatial data structures
- level of detail

---

## Bounding Boxes and Adaptive Layout

During development, another question emerged:

> How do we know that a bunny actually fits inside its grid cell?

The current scale and spacing are fixed experimental values.

A future lab can calculate the actual bounding box of the Stanford Bunny:

```text
xmin, xmax
ymin, ymax
zmin, zmax
```

and use those dimensions to determine:

- safe grid spacing
- maximum object scale
- adaptive row/column counts
- automatic fitting inside the viewport

This is deliberately not added to Lab02 because it would introduce another independent change into the Lab01/Lab02 comparison.

---

## Performance Methodology

This project is an exploratory educational benchmark rather than a formal GPU microbenchmark.

For more rigorous performance measurements, future experiments should use:

- Release builds
- fixed window size and resolution
- identical power settings
- GPU verification through `GL_VENDOR` / `GL_RENDERER`
- warm-up periods
- repeated benchmark runs
- average and median frame times
- percentile / 1% low measurements
- CPU and GPU profiling

---

## Project Structure

```text
Lab02_BunnyGrid_Instancing/
├── CMakeLists.txt
├── README.md
├── include/
│   └── bunny_data.h
├── src/
│   └── main.cpp
└── results/
    └── Lab02_Performance_Report.pdf
```

---

## Repository Progression

```text
Lab00
Basic Stanford Bunny rendering
        ↓
Lab01
Repeated draw-call benchmark
        ↓
Lab02
GPU instancing
        ↓
Future labs
Bounding volumes / adaptive layout
        ↓
Visibility and frustum culling
        ↓
Spatial data structures
        ↓
Level of Detail
        ↓
More advanced GPU-driven rendering
```

Lab01 established the performance question.

Lab02 tests one possible solution.

The fact that instancing solves only part of the performance problem provides the motivation for the next experiments.

---

## Key Takeaway

The most important lesson from this lab is not the syntax of `glDrawElementsInstanced()`.

It is the performance principle behind it:

> **Optimization should target the actual bottleneck.**

Reducing 10,000 draw calls to one draw call is architecturally significant, but the measured FPS improvement depends on what is limiting the complete rendering pipeline.

This experiment therefore serves as a bridge from basic OpenGL rendering toward performance-oriented rendering of complex scenes.
