#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "BunnyMesh.h"

static const char* vertexSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;
out vec3 vPosition;
uniform float uAngle;
uniform float uAspect;

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
    vColor = aColor;
    vPosition = p;
    gl_Position = vec4(p.x / uAspect, p.y, p.z * 0.6, 1.0);
}
)";

static const char* fragmentSource = R"(
#version 330 core
in vec3 vColor;
in vec3 vPosition;
out vec4 FragColor;

void main() {
    // Face normal estimated from screen-space position derivatives.
    // This simple flat lighting needs no normals stored in the vertex array.
    vec3 normal = normalize(cross(dFdx(vPosition), dFdy(vPosition)));
    vec3 lightDirection = normalize(vec3(-0.35, 0.70, 1.0));
    float diffuse = abs(dot(normal, lightDirection));
    float light = 0.32 + 0.68 * diffuse;
    FragColor = vec4(vColor * light, 1.0);
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
    glfwSwapInterval(1);

    if (!gladLoadGL(glfwGetProcAddress) || !GLAD_GL_VERSION_3_3) {
        std::cerr << "Could not load OpenGL 3.3\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

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
        glDrawElements(GL_TRIANGLES, (GLsizei)BunnyMesh::indexCount, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
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
