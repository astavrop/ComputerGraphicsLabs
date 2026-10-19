\# Lab 00 — Stanford Bunny Basic Rendering



This lab establishes the basic OpenGL rendering pipeline used by the

following Computer Graphics experiments.



\## Features



\- OpenGL 3.3 Core Profile

\- GLFW window and OpenGL context

\- GLAD OpenGL loader

\- Indexed Stanford Bunny mesh

\- Vertex and fragment shaders

\- Basic object rotation

\- Simple lighting/color effect

\- VBO, EBO and VAO usage



\## Purpose



The goal of this lab is to establish a reproducible base project.



Later labs extend this project with:



\- GPU instancing

\- performance benchmarking

\- Phong / Blinn-Phong lighting

\- adaptive scene layouts

\- frustum culling

\- octrees

\- Level of Detail (LOD)



\## Build



Requirements:



\- CMake

\- C++ compiler with C++17 support

\- OpenGL

\- GLFW

\- GLAD



Clone the repository:



git clone <repository-url>



Then configure and build with CMake.

## Portability Test

Lab00 was successfully cloned, configured, built, and executed on two
different Windows laptops.

The project uses CMake with FetchContent to retrieve GLFW and GLAD.

### Requirements

- Visual Studio with C++ and CMake support
- Git
- Python
- Jinja2 (`py -m pip install Jinja2`)
- OpenGL-capable graphics driver

This test confirmed that the project can be reproduced from a clean
Git clone without manually copying GLFW or GLAD libraries.

### Tested Hardware

- ASUS VivoBook Flip 14 - AMD Ryzen 5 4500U / Radeon integrated graphics
- ASUS TUF Gaming A16 - NVIDIA GeForce RTX 4050 Laptop GPU