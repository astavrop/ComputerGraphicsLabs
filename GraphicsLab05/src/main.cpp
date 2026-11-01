#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <vector>
#include <cmath>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <cstdint>
#include <cstddef>
#include <filesystem>


struct Vertex {
    float px, py, pz;
    float nx, ny, nz;
};

struct Vec3 {
    float x, y, z;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

// Convert OBJ's 1-based index to C++'s 0-based index.
// Negative OBJ indices refer backwards from the current end.
int resolveOBJIndex(int index, size_t count) {
    if (index == 0)
        throw std::runtime_error("Invalid OBJ index: 0");

    int resolved = index > 0
        ? index - 1
        : static_cast<int>(count) + index;

    if (resolved < 0 || static_cast<size_t>(resolved) >= count)
        throw std::runtime_error("OBJ index out of range");

    return resolved;
}

GLuint createDebugCubemap()
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // OpenGL face order:
    // +X, -X, +Y, -Y, +Z, -Z
    unsigned char colors[6][3] = {
        { 255, 0, 0 },       // +X Red
        { 0, 255, 0 },       // -X Green
        { 0, 0, 255 },       // +Y Blue
        { 255, 255, 0 },     // -Y Yellow
        { 255, 0, 255 },     // +Z Magenta
        { 0, 255, 255 }      // -Z Cyan
    };

    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGB8,
            1, 1,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            colors[i]
        );
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

Mesh loadOBJ(const std::string& filename) {
    std::ifstream file(filename);
    if (!file)
        throw std::runtime_error("Cannot open OBJ: " + filename);

    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    Mesh mesh;

    // Key = (position index, normal index)
    std::unordered_map<uint64_t, unsigned int> vertexLookup;

    std::string line;

    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::string type;
        stream >> type;

        if (type == "v") {
            Vec3 p;
            if (!(stream >> p.x >> p.y >> p.z))
                throw std::runtime_error("Invalid vertex position");
            positions.push_back(p);
        }
        else if (type == "vn") {
            Vec3 n;
            if (!(stream >> n.x >> n.y >> n.z))
                throw std::runtime_error("Invalid vertex normal");
            normals.push_back(n);
        }
        else if (type == "f") {
            std::vector<unsigned int> face;
            std::string token;

            while (stream >> token) {
                if (token[0] == '#')
                    break;

                // Expected token: position//normal
                size_t separator = token.find("//");
                if (separator == std::string::npos)
                    throw std::runtime_error(
                        "Expected OBJ face format v//vn");

                int rawP = std::stoi(token.substr(0, separator));
                int rawN = std::stoi(token.substr(separator + 2));

                int p = resolveOBJIndex(rawP, positions.size());
                int n = resolveOBJIndex(rawN, normals.size());

                uint64_t key =
                    (uint64_t(uint32_t(p)) << 32) |
                    uint32_t(n);

                auto found = vertexLookup.find(key);

                if (found == vertexLookup.end()) {
                    const Vec3& pos = positions[p];
                    const Vec3& norm = normals[n];

                    unsigned int newIndex =
                        static_cast<unsigned int>(
                            mesh.vertices.size());

                    mesh.vertices.push_back({
                        pos.x, pos.y, pos.z,
                        norm.x, norm.y, norm.z
                        });

                    vertexLookup[key] = newIndex;
                    face.push_back(newIndex);
                }
                else {
                    face.push_back(found->second);
                }
            }

            if (face.size() < 3)
                throw std::runtime_error("Invalid OBJ face");

            // Fan triangulation for convex polygon faces.
            for (size_t i = 1; i + 1 < face.size(); ++i) {
                mesh.indices.push_back(face[0]);
                mesh.indices.push_back(face[i]);
                mesh.indices.push_back(face[i + 1]);
            }
        }
    }

    return mesh;
}

struct GPUMesh {
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    GLsizei indexCount = 0;
};

GPUMesh uploadMesh(const Mesh& mesh) {
    GPUMesh gpu;

    gpu.indexCount =
        static_cast<GLsizei>(mesh.indices.size());

    glGenVertexArrays(1, &gpu.VAO);
    glGenBuffers(1, &gpu.VBO);
    glGenBuffers(1, &gpu.EBO);

    glBindVertexArray(gpu.VAO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, gpu.VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        mesh.vertices.size() * sizeof(Vertex),
        mesh.vertices.data(),
        GL_STATIC_DRAW
    );

    // Element/index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu.EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        mesh.indices.size() * sizeof(unsigned int),
        mesh.indices.data(),
        GL_STATIC_DRAW
    );

    // Attribute 0: position
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, px)
    );
    glEnableVertexAttribArray(0);

    // Attribute 1: normal
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, nx)
    );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return gpu;
}

struct SphereMesh
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

SphereMesh generateSphere(float radius, int stacks, int sectors)
{
    SphereMesh sphere;

    constexpr float PI = 3.14159265359f;

    for (int i = 0; i <= stacks; ++i)
    {
        float theta = PI * i / stacks;

        for (int j = 0; j <= sectors; ++j)
        {
            float phi = 2.0f * PI * j / sectors;

            float x = radius * sin(theta) * cos(phi);
            float y = radius * cos(theta);
            float z = radius * sin(theta) * sin(phi);

            // Outward normal for a sphere centered at the origin
            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;

            // Position (3 floats) + Normal (3 floats)
            sphere.vertices.insert(
                sphere.vertices.end(),
                { x, y, z, nx, ny, nz }
            );
        }
    }

    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < sectors; ++j)
        {
            unsigned int A = i * (sectors + 1) + j;
            unsigned int B = A + 1;
            unsigned int C = (i + 1) * (sectors + 1) + j;
            unsigned int D = C + 1;

            sphere.indices.insert(
                sphere.indices.end(),
                { A, C, B, B, C, D }
            );
        }
    }

    return sphere;
}

int main() {


    struct DirectionalLight {
        float direction[3];
        float color[3];
        float intensity;
    };

    DirectionalLight sunlight = {
        { -0.5f, -0.5f, -1.0f },
        { 1.0f, 1.0f, 1.0f },
        1.0f
    };

    bool enableAmbient = true;
    bool enableDiffuse = true;
    bool enableSpecular = true;
    bool autoRotate = false;
    float rotationDegrees = 0.0f;
    struct Material {
        float ambient[3];
        float diffuse[3];
        float specular[3];
        float shininess;
    };

    Material gold = {
        { 0.247f, 0.200f, 0.075f },  // Ambient
        { 0.752f, 0.606f, 0.226f },  // Diffuse
        { 1.000f, 0.800f, 0.300f },  // Specular
        128.0f                     // Shininess
    };

    float bunnyScale = 0.5f; // Declare outside the render loop

    // Position (x,y,z)     Normal (nx,ny,nz)
    float vertices[] = {
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.5f, 1.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    float diffuseStrength = 1.0f;
    float specularStrenght = 0.5;
    float ambientStrength = 0.20f;
    std::cout << sizeof(vertices) << " bytes\n";

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    // Step 1 uses OpenGL functions available without GLAD.
    GLFWwindow* window = glfwCreateWindow(800, 600, "Lab 05 - Phong Lighting Sphere", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

 

    const char* fragmentShaderSource = R"(
#version 330 core
uniform float specularStrenght;
uniform float ambientStrength;
uniform float diffuseStrength;
out vec4 FragColor;
in vec3 vNormal;
in vec3 LocalPos;
in vec3 FragPos;
uniform bool showCheckerboard;
uniform float checkerFrequency;
uniform bool enableCubemap;
uniform float reflectionStrength;
uniform samplerCube skybox;
uniform vec3 cameraPos;
struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
};


uniform PointLight pointLight;
uniform int lightType;

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform DirectionalLight light;


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform bool enableAmbient;
uniform bool enableDiffuse;
uniform bool enableSpecular;

void main()
{
    vec3 L;
    vec3 lightColor;
    float lightIntensity;
    
    if (lightType == 0){
    // Directional light
    L = normalize(-light.direction);
    lightColor = light.color;
    lightIntensity = light.intensity;
}
else
{
    // Point light
    L =normalize(pointLight.position - FragPos);
    lightColor =pointLight.color;
    lightIntensity=pointLight.intensity;
}
     
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vec3(0.0, 0.0, 1.0));
    float diffuse = max(dot(N, L), 0.0);
   
    
    
   vec3 diffuseColor = diffuseStrength *material.diffuse * lightColor * lightIntensity * diffuse;

   

   vec3 R = reflect(-L, N);
   



float spec = pow(max(dot(R, V), 0.0), material.shininess);

vec3 specular = specularStrenght * spec*material.specular * lightColor * lightIntensity;

vec3 ambient = ambientStrength * material.ambient;

vec3 finalColor = vec3(0.0);

if (enableAmbient)
    finalColor += ambient;

if (enableDiffuse)
    finalColor += diffuseColor;

if (enableSpecular)
    finalColor += specular;


if (showCheckerboard)
{
    vec3 cell = floor(LocalPos * checkerFrequency);

    float parity = mod(cell.x + cell.y + cell.z, 2.0);

    float brightness = mix(0.45, 1.30, parity);

    finalColor *= brightness;
}

if (enableCubemap)
{
    // Surface normal
    vec3 N = normalize(vNormal);

    // Incident viewing direction
    vec3 I = normalize(FragPos - cameraPos);

    // Reflection vector
    vec3 R = reflect(I, N);

    // Sample the environment
    vec3 environmentColor = texture(skybox, R).rgb;

    // Combine Phong and environment reflections
    finalColor = mix(
        finalColor,
        environmentColor,
        reflectionStrength
    );
}
FragColor = vec4(finalColor, 1.0);
}
)";

    const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
uniform float rotationAngle;
uniform float aspectRatio;
out vec3 vNormal;
out vec3 LocalPos;
out vec3 FragPos;

uniform float bunnyScale;
void main()
{

    float c = cos(rotationAngle);
    float s = sin(rotationAngle);

    mat3 rotationY = mat3(
         c, 0.0, -s,
       0.0, 1.0, 0.0,
         s, 0.0,  c
    );

    
    // After rotating the bunny:
vec3 worldPosition = rotationY * aPosition;

vec3 clipPosition = worldPosition *bunnyScale;

clipPosition.x /= aspectRatio;

    vec3 rotatedNormal   = rotationY * aNormal;
    FragPos=clipPosition;
    vNormal = rotatedNormal;
    LocalPos = aPosition;
    
    gl_Position = vec4(clipPosition, 1.0);
    
}
)";
    bool enableCubemap = false;
    float reflectionStrength = 0.35f;
    // Load OpenGL functions using GLAD 2
    int version = gladLoadGL(
        reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)
    );

    if (version == 0)
    {
        std::cerr << "Failed to initialize GLAD!\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL version: "
        << GLAD_VERSION_MAJOR(version) << "."
        << GLAD_VERSION_MINOR(version) << "\n";

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    GLuint debugCubemap = createDebugCubemap();
    std::cout << "Working directory: "
        << std::filesystem::current_path()
        << std::endl;

    std::cout << "OBJ exists: "
        << std::filesystem::exists("assets/lab05_bunny.obj")
        << std::endl;

    Mesh bunny = loadOBJ("assets/lab05_bunny.obj");

    GPUMesh bunnyGPU = uploadMesh(bunny);

    std::cout
        << "Vertices: " << bunny.vertices.size() << '\n'
        << "Triangles: " << bunny.indices.size() / 3 << '\n';

    /*SphereMesh sphere = generateSphere(0.4f, 32, 64);

    unsigned int sphereVAO, sphereVBO, sphereEBO;

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    // Vertex data
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sphere.vertices.size() * sizeof(float),
        sphere.vertices.data(),
        GL_STATIC_DRAW
    );

    // Triangle indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sphere.indices.size() * sizeof(unsigned int),
        sphere.indices.data(),
        GL_STATIC_DRAW
    );

    // Position attribute
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);*/

    static int lightType = 0;
    // 0 = Stationary Light
    // 1 = Moving Point Light
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    int success = 0;

    glGetShaderiv(
        vertexShader,
        GL_COMPILE_STATUS,
        &success
    );

    if (success == GL_FALSE)
    {
        char infoLog[512];

        glGetShaderInfoLog(
            vertexShader,
            512,
            nullptr,
            infoLog
        );

        std::cerr << "Vertex shader compilation failed:\n"
            << infoLog << std::endl;
    }

        unsigned int fragmentShader =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        nullptr
    );

    glCompileShader(fragmentShader);

    int fragmentSuccess = 0;

    glGetShaderiv(
        fragmentShader,
        GL_COMPILE_STATUS,
        &fragmentSuccess
    );

    if (fragmentSuccess == GL_FALSE)
    {
        char infoLog[512];

        glGetShaderInfoLog(
            fragmentShader,
            512,
            nullptr,
            infoLog
        );

        std::cerr << "Fragment shader compilation failed:\n"
            << infoLog << std::endl;
    }

    // 1. Create an empty shader program
    unsigned int shaderProgram = glCreateProgram();

    // 2. Attach our compiled shaders
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // 3. Link the program
    glLinkProgram(shaderProgram);

    int linkSuccess = 0;

    glGetProgramiv(
        shaderProgram,
        GL_LINK_STATUS,
        &linkSuccess
    );

    if (linkSuccess == GL_FALSE)
    {
        char infoLog[512];

        glGetProgramInfoLog(
            shaderProgram,
            512,
            nullptr,
            infoLog
        );

        std::cerr << "Shader program linking failed:\n"
            << infoLog << std::endl;
    }

    rotationDegrees = 0.0f;
     autoRotate = false;
    float rotationSpeed = 45.0f; // Degrees per second
    glfwSwapInterval(1);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // Background clear color for the lighting demonstration.
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, debugCubemap);

        glUniform1i(
            glGetUniformLocation(shaderProgram, "skybox"),
            0
        );

        glBindVertexArray(bunnyGPU.VAO);

        glDrawElements(
            GL_TRIANGLES,
            bunnyGPU.indexCount,
            GL_UNSIGNED_INT,
            nullptr
        );

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Graphics Lab 05");
        ImGui::Text("Phong Lighting - Sphere Laboratory");
        ImGui::SliderFloat(
            "Diffuse Strength",
            &diffuseStrength,
            0.0f,
            1.0f
        );

        ImGui::SeparatorText("Environment Mapping");

        ImGui::Checkbox(
            "Enable Cubemap Reflections",
            &enableCubemap
        );

        if (enableCubemap)
        {
            ImGui::SliderFloat(
                "Reflection Strength",
                &reflectionStrength,
                0.0f,
                1.0f
            );
        }

        ImGui::SliderFloat(
            "Y Rotation",
            &rotationDegrees,
            -180.0f,
            180.0f
        );

        ImGui::Separator();
        ImGui::Text("Rotation Controls");

        ImGui::Checkbox("Auto Rotate", &autoRotate);
        if (!autoRotate)
        {
            ImGui::SliderFloat(
                "Y Rotation",
                &rotationDegrees,
                -180.0f,
                180.0f
            );
        }
        ImGui::SliderFloat(
            "Rotation Speed",
            &rotationSpeed,
            0.0f,
            180.0f
        );
        static bool wireframe = false;
        ImGui::SliderFloat(
            "Bunny Scale",
            &bunnyScale,
            0.05f,
            2.0f
        );
        ImGui::Checkbox("Wireframe", &wireframe);
        ImGui::Separator();
        ImGui::Text("Lighting Components");

        ImGui::Checkbox("Ambient", &enableAmbient);
        ImGui::Checkbox("Diffuse", &enableDiffuse);
        ImGui::Checkbox("Specular", &enableSpecular);

        ImGui::SliderFloat(
            "Ambient Strength",
            &ambientStrength,
            0.0f,
            1.0f
        );
        ImGui::SliderFloat(
            "Shininess",
            &gold.shininess,
            1.0f,
            256.0f
        );

        ImGui::ColorEdit3("Specular Color", gold.specular);
        ImGui::ColorEdit3("Ambient", gold.ambient);
        ImGui::ColorEdit3("Diffuse", gold.diffuse);
        ImGui::SliderFloat(
            "Specular Strength",
            &specularStrenght,
            0.0f,
            1.0f
        );

        static bool showCheckerboard = true;
        static float checkerFrequency = 4.0f;

        ImGui::Checkbox("Show Checkerboard", &showCheckerboard);

        ImGui::SliderFloat(
            "Checker Frequency",
            &checkerFrequency,
            1.0f,
            20.0f
        );

        ImGui::SeparatorText("Light Source");

        ImGui::RadioButton("Stationary Light", &lightType, 0);
        ImGui::RadioButton("Moving Point Light", &lightType, 1);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glUniform1f(
            glGetUniformLocation(shaderProgram, "ambientStrength"),
            ambientStrength
        );

        static double previousTime = glfwGetTime();

        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - previousTime);
        previousTime = currentTime;

        if (autoRotate)
        {
            rotationDegrees += rotationSpeed * deltaTime;

            if (rotationDegrees >= 360.0f)
                rotationDegrees -= 360.0f;
        }

        float rotationRadians =
            rotationDegrees * 3.14159265f / 180.0f;

        glUseProgram(shaderProgram);



        glUniform1f(
            glGetUniformLocation(shaderProgram, "rotationAngle"),
            rotationRadians
        );

        

        if (height > 0)
        {
            glUniform1f(
                glGetUniformLocation(shaderProgram, "aspectRatio"),
                static_cast<float>(width) / height
            );
        }


        float time = static_cast<float>(glfwGetTime());

        float lightX = 1.5f * cos(time * 0.8f);
        float lightY = 0.5f;
        float lightZ = 1.5f * sin(time * 0.8f);

        glUniform1f(
            glGetUniformLocation(shaderProgram, "bunnyScale"),
            bunnyScale
        );

        glUniform1i(
            glGetUniformLocation(shaderProgram, "enableCubemap"),
            enableCubemap ? 1 : 0
        );

        glUniform1f(
            glGetUniformLocation(shaderProgram, "reflectionStrength"),
            reflectionStrength
        );

        glUniform1i(
            glGetUniformLocation(shaderProgram, "lightType"),
            lightType
        );

        glUniform1i(
            glGetUniformLocation(shaderProgram, "enableCubemap"),
            enableCubemap ? 1 : 0
        );

        glUniform1f(
            glGetUniformLocation(shaderProgram, "reflectionStrength"),
            reflectionStrength
        );

        glUniform3f(
            glGetUniformLocation(shaderProgram, "cameraPos"),
            0.0f, 0.0f, 5.0f
        );

        glUniform3f(
            glGetUniformLocation(shaderProgram, "pointLight.position"),
            lightX, lightY, lightZ
        );

        glUniform3f(
            glGetUniformLocation(shaderProgram, "pointLight.color"),
            1.0f, 1.0f, 1.0f
        );

        glUniform1f(
            glGetUniformLocation(shaderProgram, "pointLight.intensity"),
            1.0f
        );


        glUniform3f(
            glGetUniformLocation(shaderProgram, "light.direction"),
            sunlight.direction[0],
            sunlight.direction[1],
            sunlight.direction[2]
        );

        glUniform3f(
            glGetUniformLocation(shaderProgram, "light.color"),
            sunlight.color[0],
            sunlight.color[1],
            sunlight.color[2]
        );

        glUniform1i(
            glGetUniformLocation(shaderProgram, "showCheckerboard"),
            showCheckerboard ? 1 : 0
        );

        glUniform1f(
            glGetUniformLocation(shaderProgram, "checkerFrequency"),
            checkerFrequency
        );

        glUniform1f(
            glGetUniformLocation(shaderProgram, "light.intensity"),
            sunlight.intensity
        );


        glUniform1i(
            glGetUniformLocation(shaderProgram, "enableAmbient"),
            enableAmbient ? 1 : 0
        );

        glUniform1f(
            glGetUniformLocation(shaderProgram, "diffuseStrength"),
            diffuseStrength
        );

        glUniform1i(
            glGetUniformLocation(shaderProgram, "enableDiffuse"),
            enableDiffuse ? 1 : 0
        );

        glUniform1i(
            glGetUniformLocation(shaderProgram, "enableSpecular"),
            enableSpecular ? 1 : 0
        );

        glUniform1f(glGetUniformLocation(shaderProgram, "specularStrenght"), specularStrenght);

        glUniform3fv(
            glGetUniformLocation(shaderProgram, "material.ambient"),
            1, gold.ambient
        );

        glUniform3fv(
            glGetUniformLocation(shaderProgram, "material.diffuse"),
            1, gold.diffuse
        );

        glUniform3fv(
            glGetUniformLocation(shaderProgram, "material.specular"),
            1, gold.specular
        );

        glUniform1f(
            glGetUniformLocation(shaderProgram, "material.shininess"),
            gold.shininess
        );

        glPolygonMode(
            GL_FRONT_AND_BACK,
            wireframe ? GL_LINE : GL_FILL
        );

        glBindVertexArray(bunnyGPU.VAO);

        glDrawElements(
            GL_TRIANGLES,
            bunnyGPU.indexCount,
            GL_UNSIGNED_INT,
            nullptr
        );

        glBindVertexArray(0);
       

        /*glBindVertexArray(sphereVAO);

        glDrawElements(
            GL_TRIANGLES,
            static_cast<GLsizei>(sphere.indices.size()),
            GL_UNSIGNED_INT,
            nullptr
        );*/

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /*glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);

    }*/

    glDeleteBuffers(1, &bunnyGPU.EBO);
    glDeleteBuffers(1, &bunnyGPU.VBO);
    glDeleteVertexArrays(1, &bunnyGPU.VAO);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
