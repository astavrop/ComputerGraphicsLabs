# Dependency options

The code uses the **same APIs** as Lab01/Lab02: GLFW and classic GLAD1 (`<glad/glad.h>`, `gladLoadGLLoader`).

By default the CMake project will try to find GLFW, fetch GLFW 3.4 if necessary, and fetch three GLAD1 generated sources from LearnOpenGL (upstream). An internet connection may be required once.

Offline GLAD option (recommended if you have working dependencies from Lab02):

```text
third_party/glad/include/glad/glad.h
third_party/glad/include/KHR/khrplatform.h
third_party/glad/src/glad.c
```

If GLFW is already installed or available through your existing Visual Studio/vcpkg toolchain, configure with `-DLAB03_FETCH_GLFW=OFF`. Pass `-Dglfw3_DIR=...` if needed.

Upstream GLFW: https://github.com/glfw/glfw
Upstream GLAD1 generated files: https://github.com/JoeyDeVries/LearnOpenGL

This ZIP contains our source and the mesh from your previous lab, not copies of GLFW/GLAD binary libraries.
