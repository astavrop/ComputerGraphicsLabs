\# Computer Graphics Labs



A collection of practical Computer Graphics experiments implemented in

C++ and modern OpenGL.



The purpose of this repository is to explore the graphics rendering

pipeline through small, reproducible experiments, starting from basic

mesh rendering and gradually moving toward more advanced real-time

rendering and scene optimization techniques.



\## Technologies



\- C++

\- OpenGL 3.3 Core Profile

\- GLSL

\- GLFW

\- GLAD

\- CMake

\- Visual Studio



\## Labs



\### Lab 00 — Basic Stanford Bunny Rendering



Initial OpenGL setup and rendering pipeline.



Topics:



\- OpenGL context creation

\- Vertex Array Objects (VAO)

\- Vertex Buffer Objects (VBO)

\- Element Buffer Objects (EBO)

\- Indexed triangle rendering

\- Vertex and fragment shaders

\- Basic transformations

\- Rotation

\- Basic color / lighting effect

\- Stanford Bunny mesh



This lab serves as the base project for the following experiments.



\### Lab 01 — Stanford Bunny Rendering Benchmark



Performance experiment using multiple instances of the Stanford Bunny.



Scene sizes:



\- 1 bunny

\- 100 bunnies

\- 1,000 bunnies

\- 10,000 bunnies



The experiment compares frame rate and frame time while increasing

scene complexity.



The initial implementation intentionally uses separate draw calls to

establish a baseline before introducing GPU instancing and other

optimizations.



\## Planned Experiments



Future labs will explore:



\- GPU instanced rendering

\- Model, View and Projection matrices

\- Phong / Blinn-Phong lighting

\- Large scene rendering

\- Frustum culling

\- Spatial data structures

\- Octrees

\- Level of Detail (LOD)

\- Performance profiling

\- CPU-bound vs GPU-bound rendering

\- Rendering optimization



\## Goal



The goal is not only to produce rendered images, but also to understand

how rendering algorithms, scene complexity, CPU/GPU communication and

GPU architecture affect real-time performance.



Each lab builds on the previous experiments and documents both the

implementation and the observed results.



\## Hardware Experiments



Some experiments are executed on both integrated and discrete GPUs in

order to compare their behavior under different rendering workloads.



Results are documented inside the corresponding lab directories.



\## Build



Each lab contains its own build instructions and dependencies.



General requirements:



\- C++17 compatible compiler

\- CMake

\- OpenGL capable GPU and driver



See the README inside each lab directory for specific instructions.



\## Repository Structure



&#x20;   ComputerGraphicsLabs/

&#x20;   ├── README.md

&#x20;   ├── .gitignore

&#x20;   ├── Lab00\_BasicBunny/

&#x20;   │   ├── README.md

&#x20;   │   ├── CMakeLists.txt

&#x20;   │   ├── src/

&#x20;   │   └── include/

&#x20;   │

&#x20;   └── Lab01\_BunnyBenchmark/

&#x20;       ├── README.md

&#x20;       ├── CMakeLists.txt

&#x20;       ├── src/

&#x20;       ├── include/

&#x20;       └── docs/



\## Status



Work in progress.



The repository is being developed incrementally as a practical study of

real-time Computer Graphics.

