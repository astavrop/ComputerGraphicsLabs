# Computer Graphics Labs



A collection of practical Computer Graphics experiments implemented in

C++ and modern OpenGL.



The purpose of this repository is to explore the graphics rendering

pipeline through small, reproducible experiments, starting from basic

mesh rendering and gradually moving toward more advanced real-time

rendering and scene optimization techniques.



## Background & Motivation

This repository is a personal return to computer graphics, inspired in part by topics I studied at the University of Paderborn more than a decade ago, including *Advanced Rendering* and *Algorithms for Highly Complex Virtual Scenes*.

The labs revisit concepts such as the graphics pipeline, GPU programming, scene hierarchies, spatial data structures, visibility and culling, mapping techniques, advanced shading, global illumination, and rendering of complex scenes using modern C++, OpenGL and GLSL.

The original university material serves as thematic and educational inspiration rather than as a direct reproduction of the original assignments. The implementations, experiments and organization of this repository are newly developed.

A major difference from my student days is the development process itself: these labs are being created through an iterative collaboration with generative AI (ChatGPT), used as a learning, programming and debugging assistant. Each experiment is implemented, tested, evaluated and progressively refined by the author.

*In 2013, I studied these ideas from lecture slides and implemented graphics assignments without generative AI. More than a decade later, I am revisiting the same field with modern hardware, modern OpenGL, and AI as an interactive learning partner.*

---

## Technologies



- C++

- OpenGL 3.3 Core Profile

- GLSL

- GLFW

- GLAD

- CMake

- Visual Studio



## Labs



### Lab 00 — Basic Stanford Bunny Rendering



Initial OpenGL setup and rendering pipeline.



Topics:



- OpenGL context creation

- Vertex Array Objects (VAO)

- Vertex Buffer Objects (VBO)

- Element Buffer Objects (EBO)

- Indexed triangle rendering

- Vertex and fragment shaders

- Basic transformations

- Rotation

- Basic color / lighting effect

- Stanford Bunny mesh



This lab serves as the base project for the following experiments.



### Lab 01 — Stanford Bunny Rendering Benchmark



Performance experiment using multiple instances of the Stanford Bunny.



Scene sizes:



- 1 bunny

- 100 bunnies

- 1,000 bunnies

- 10,000 bunnies



The experiment compares frame rate and frame time while increasing

scene complexity.



The initial implementation intentionally uses separate draw calls to

establish a baseline before introducing GPU instancing and other

optimizations.



### Lab 02 — Stanford Bunny GPU Instancing

Controlled follow-up to Lab01 using GPU instancing.

The experiment replaces repeated per-object `glDrawElements()` calls with a single `glDrawElementsInstanced()` call. Instance positions are calculated in the vertex shader using `gl_InstanceID`.

The same 1, 100, 1,000 and 10,000 bunny workloads are tested on both integrated AMD graphics and an NVIDIA GeForce RTX 4050 Laptop GPU, allowing direct comparison with the Lab01 brute-force baseline.

Topics:

- GPU instancing
- `glDrawElementsInstanced()`
- `gl_InstanceID`
- CPU/driver draw-call overhead
- CPU-to-GPU submission
- workload scaling
- integrated vs discrete GPU behavior
- controlled performance comparison with Lab01

The measurements show that reducing draw calls can provide a substantial benefit for some workloads, but the improvement depends on the actual rendering bottleneck.

---

## Planned Experiments



Future labs will explore:




- Model, View and Projection matrices

- Phong / Blinn-Phong lighting

- Large scene rendering

- Frustum culling

- Spatial data structures

- Octrees

- Level of Detail (LOD)

- Performance profiling

- CPU-bound vs GPU-bound rendering

- Rendering optimization



## Goal



The goal is not only to produce rendered images, but also to understand

how rendering algorithms, scene complexity, CPU/GPU communication and

GPU architecture affect real-time performance.



Each lab builds on the previous experiments and documents both the

implementation and the observed results.



## Hardware Experiments



Some experiments are executed on both integrated and discrete GPUs in

order to compare their behavior under different rendering workloads.



Results are documented inside the corresponding lab directories.



## Build



Each lab contains its own build instructions and dependencies.



General requirements:



- C++17 compatible compiler

- CMake

- OpenGL capable GPU and driver



See the README inside each lab directory for specific instructions.



## Repository Structure



    ComputerGraphicsLabs/

    ├── README.md

    ├── .gitignore

    ├── Lab00_BasicBunny/

    │   ├── README.md

    │   ├── CMakeLists.txt

    │   ├── src/

    │   └── include/

    │

    └── Lab01_BunnyBenchmark/

        ├── README.md

        ├── CMakeLists.txt

        ├── src/

        ├── include/

        └── docs/



## Status



Work in progress.



The repository is being developed incrementally as a practical study of

real-time Computer Graphics.

