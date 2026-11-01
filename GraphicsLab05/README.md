# Graphics Lab 05 — Sphere Lighting Laboratory

**Intermediate stage before the metallic Stanford Bunny.** This is the sphere implementation from the Lab 05 learning sequence, packaged separately so students can investigate lighting on simple geometry before moving to the Bunny mesh.

## Features

- Procedurally generated sphere mesh (vertices, normals and indexed triangles)
- GLSL vertex and fragment shaders with Phong reflection-model components: **ambient, diffuse and specular**
- Adjustable shininess exponent and material colors
- Directional (stationary) light or orbiting point light
- Automatic or manual Y-axis sphere rotation and adjustable rotation speed
- Optional 3D checkerboard pattern and frequency control
- Wireframe toggle and interactive Dear ImGui controls

## Build and run

Requires a C++17 compiler, **CMake 3.20+**, Git, internet access for CMake FetchContent, and an OpenGL 3.3-capable graphics environment.

```bash
cmake -S . -B build
cmake --build build --config Debug
```

Run the `GraphicsLab05_Sphere` executable (in a multi-config build, it may be under `build/Debug/`). Alternatively, open this folder in Visual Studio with CMake support and select the `GraphicsLab05_Sphere` target.

CMake downloads GLFW 3.4, GLAD 2.0.8 and Dear ImGui 1.91.9b during configuration.

## Suggested learning sequence

1. Turn on **Ambient** alone and observe its constant contribution.
2. Enable **Diffuse** and compare the sphere's light-facing and dark-facing regions.
3. Enable **Specular**; vary **Shininess** from low (broad highlight) to high (concentrated highlight).
4. Switch from **Stationary Light** to **Moving Point Light** and observe the changing illumination.
5. Enable sphere rotation, checkerboard and wireframe to explore surface normals, geometry and shading.
6. Carry the same lighting concepts forward to the **metallic Stanford Bunny** stage.

The checkerboard is a procedural pattern based on local object coordinates; it is not itself the diffuse-lighting term.

## Source layout

- `src/main.cpp` — sphere generation, GLSL shaders, rendering loop and ImGui controls
- `CMakeLists.txt` — CMake build and third-party dependencies

## Notes

This folder preserves the user's sphere-stage implementation. It is intended as a standalone intermediate lab, not as a polished final rendering engine. The project has not been compiled or run in this packaging environment. No generated build files or third-party source trees are included.
