#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "bunny_data.h"

static const char* vertexSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;
uniform float uAngle;
uniform float uAspect;
uniform vec2 uOffset;
uniform float uScale;

void main() {
    float c = cos(uAngle);
    float s = sin(uAngle);
    // mat3 constructor takes columns in GLSL.

    mat3 rotateY=mat3(c, 0.0, -s,
                        0.0, 1.0, 0.0,
                        s, 0.0, c);

    mat3 rotateX= mat3(1.0, 0.0, 0.0,
                        0.0,c, -s,
                        0.0, s, c);

    mat3 rotateZ=mat3(c, -s, 0.0,
                        s, c, 0.0,
                        0.0, 0.0, 1.0);

    vec3 p = rotateY* aPos;
    
    // Base cyan/blue color
vec3 baseColor = vec3(0.15, 0.65, 0.85);

// Simple shiny variation depending on vertex position
float shine = 0.45 + 0.55 * (aPos.y + 0.5);

shine = clamp(shine, 0.0, 1.0);

vColor = baseColor * shine;


 gl_Position= vec4(
    (p.x * uScale) / uAspect + uOffset.x,
    (p.y * uScale) + uOffset.y,
    p.z * 0.6 * uScale,
    1.0
);
}
)";

static const char* fragmentSource = R"(
#version 330 core
in vec3 vColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}

)";

static GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        char log[2048] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compile failed: " << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

int main() {

    int bunnyCount = 1;
    int choice = 0;

    std::cout << "=============================\n";
    std::cout << "      BUNNY BENCHMARK\n";
    std::cout << "=============================\n\n";

    std::cout << "How many bunnies would you like to render?\n\n";

    std::cout << "1. 1 bunny\n";
    std::cout << "2. 100 bunnies\n";
    std::cout << "3. 1000 bunnies\n";
    std::cout << "4. 10000 bunnies\n\n";

    std::cout << "Select: ";
    std::cin >> choice;

    switch (choice)
    {
    case 1:
        bunnyCount = 1;
        break;

    case 2:
        bunnyCount = 100;
        break;

    case 3:
        bunnyCount = 1000;
        break;

    case 4:
        bunnyCount = 10000;
        break;

    default:
        std::cout << "Invalid selection. Using 1 bunny.\n";
        bunnyCount = 1;
        break;
    }

    std::cout << "\nRendering "
        << bunnyCount
        << " bunnies...\n\n";

    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(900, 700, "Modern OpenGL - Stanford Bunny", nullptr, nullptr);
    if (!window) {
        std::cerr << "Could not create OpenGL 3.3 window\n";
        glfwTerminate();
        return 1;
    }



    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); //disable vsync for benchmarking 

    if (!gladLoadGL(glfwGetProcAddress) || !GLAD_GL_VERSION_3_3) {
        std::cerr << "Could not load OpenGL 3.3\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;

    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!vs || !fs) {
        glDeleteShader(vs);
        glDeleteShader(fs);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint linked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        char log[2048] = {};
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Shader program link failed: " << log << '\n';
        glDeleteProgram(program);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    GLuint vao = 0, vbo = 0, ebo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BunnyMesh::vertices), BunnyMesh::vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BunnyMesh::indices), BunnyMesh::indices, GL_STATIC_DRAW);

    // Each vertex is 6 floats: position x/y/z, then RGB.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST); // Crucial for a 3D mesh.
    glUseProgram(program);
    GLint angleLocation = glGetUniformLocation(program, "uAngle");
    GLint aspectLocation = glGetUniformLocation(program, "uAspect");
    GLint offsetLocation = glGetUniformLocation(program, "uOffset");
    std::cout << "offsetLocation = " << offsetLocation << std::endl;
    GLint scaleLocation = glGetUniformLocation(program, "uScale");
    double previousTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        int width = 1, height = 1;
        glfwGetFramebufferSize(window, &width, &height);
        if (height == 0) { glfwPollEvents(); continue; }
        glViewport(0, 0, width, height);
        glClearColor(0.06f, 0.09f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glUniform1f(angleLocation, (float)glfwGetTime() * 0.6f + 0.6f);
        glUniform1f(aspectLocation, (float)width / (float)height);
        glBindVertexArray(vao);
        //glDrawElements(GL_TRIANGLES, (GLsizei)BunnyMesh::indexCount, GL_UNSIGNED_INT, nullptr);
        int columns = 10;

        for (int i = 0; i < bunnyCount; i++)
        {
            int row = i / columns;
            int col = i % columns;

            float x = -0.9f + col * 0.20f;
            float y = 0.9f - row * 0.20f;

            glUniform2f(offsetLocation, x, y);
            glUniform1f(scaleLocation, 0.10f);

            glDrawElements(
                GL_TRIANGLES,
                (GLsizei)BunnyMesh::indexCount,
                GL_UNSIGNED_INT,
                nullptr
            );
        }


        glfwSwapBuffers(window);

        frameCount++;

        double currentTime = glfwGetTime();
        double elapsed = currentTime - previousTime;

        if (elapsed >= 1.0)
        {
            double fps = frameCount / elapsed;
            double frameTime = 1000.0 / fps;

            std::cout << "FPS: " << fps
                << " | Frame time: " << frameTime
                << " ms" << std::endl;

            frameCount = 0;
            previousTime = currentTime;
        }

        glfwPollEvents();
    }

        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
   
}
