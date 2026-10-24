#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "bunny_data.h"
#include "Lab03Math.h"
#ifdef LAB03_USE_STUDENT_MATH
#include "Lab03Student.h"
#endif

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#ifndef LAB03_SHADER_DIR
#define LAB03_SHADER_DIR "shaders"
#endif
#include <fstream>
static int renderMode = 1; // Start with checkerboard
float checkerColorA[3] = { 0.76f, 0.54f, 0.32f };
float checkerColorB[3] = { 0.41f, 0.25f, 0.15f };
float solidColor[3] = {
    0.0f, 0.8f, 0.9f
}; // Initial cyan
float checkerFrequency = 5.0f;

float pulseSpeed = 1.0f;
float glowIntensity = 1.0f;

float fireflyDark[3] = {
    0.35f, 0.20f, 0.02f
};

float fireflyBright[3] = {
    1.0f, 0.95f, 0.50f
};

namespace {
std::string readText(const std::string& p) {
    std::ifstream f(p);
    if (!f) throw std::runtime_error("Cannot open shader: " + p);
    return std::string(std::istreambuf_iterator<char>(f), {});
}
GLuint compile(GLenum stage, const std::string& source) {
    GLuint shader=glCreateShader(stage);
    const char* s=source.c_str(); glShaderSource(shader,1,&s,nullptr); glCompileShader(shader);
    GLint ok=0; glGetShaderiv(shader,GL_COMPILE_STATUS,&ok);
    if(!ok) { char buf[4096]{}; glGetShaderInfoLog(shader,sizeof(buf),nullptr,buf);
        glDeleteShader(shader); throw std::runtime_error(buf); }
    return shader;
}
GLuint shaderProgram() {
    const std::string root=LAB03_SHADER_DIR;
    const GLuint v=compile(GL_VERTEX_SHADER,readText(root + "/bunny.vert"));
    const GLuint f=compile(GL_FRAGMENT_SHADER,readText(root + "/bunny.frag"));
    GLuint p=glCreateProgram(); glAttachShader(p,v); glAttachShader(p,f); glLinkProgram(p);
    glDeleteShader(v); glDeleteShader(f);
    GLint ok=0; glGetProgramiv(p,GL_LINK_STATUS,&ok);
    if(!ok){ char buf[4096]{}; glGetProgramInfoLog(p,sizeof(buf),nullptr,buf);
        glDeleteProgram(p); throw std::runtime_error(buf); }
    return p;
}
struct App {
    GLFWwindow* window=nullptr;
    int count=100, framebufferWidth=800, framebufferHeight=600;
    bool rotate=true, outlines=false, wireframe=false, vsync=false, layoutDirty=true;
    std::string typed;
    lab03::BoundingSphere sphere{};
    lab03::GridLayout grid{};
    GLuint program=0, vao=0, vbo=0, ebo=0, circleVao=0, circleVbo=0;
    double timeOrigin=0.0;
};
int getInitialCount() {
    std::cout << "Lab03 | How many Bunnies (1..10000)? [100]: " << std::flush;
    std::string input;
    if(!std::getline(std::cin,input) || input.empty()) return 100;
    try { const long long n=std::stoll(input); if(n>=1 && n<=10000) return (int)n; }
    catch(...) {}
    std::cerr << "Invalid count. Using 100.\n"; return 100;
}
void updateLayout(App& a) {
    const int width=std::max(1,a.framebufferWidth), height=std::max(1,a.framebufferHeight);
    const double H=2.0, W=H*static_cast<double>(width)/height;
#ifdef LAB03_USE_STUDENT_MATH
    a.grid=lab03::student::computeGridLayout(a.count,W,H);
    a.grid.scale=lab03::student::computeFixedScale(a.grid,a.sphere.radius);
#else
    a.grid=lab03::computeGridLayout(a.count,W,H);
    a.grid.scale=lab03::computeFixedScale(a.grid,a.sphere.radius);
#endif
    std::cout << "Grid: " << a.count << " Bunnies | " << a.grid.columns
              << " columns x " << a.grid.rows << " rows | fixed scale "
              << std::fixed << std::setprecision(6) << a.grid.scale << '\n';
    a.layoutDirty=false;
}
void framebufferSize(GLFWwindow* w,int width,int height) {
    App& a=*static_cast<App*>(glfwGetWindowUserPointer(w));
    a.framebufferWidth=width; a.framebufferHeight=height;
    glViewport(0,0,width,height); a.layoutDirty=true;
}
void keyCallback(GLFWwindow* w,int key,int,int action,int) {
    if(action!=GLFW_PRESS) return;
    App& a=*static_cast<App*>(glfwGetWindowUserPointer(w));
    if(key==GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(w,GLFW_TRUE);
    if(key==GLFW_KEY_R) a.rotate=!a.rotate;
    if(key==GLFW_KEY_B) a.outlines=!a.outlines;
    if(key==GLFW_KEY_W) a.wireframe=!a.wireframe;
    if(key==GLFW_KEY_V){a.vsync=!a.vsync; glfwSwapInterval(a.vsync?1:0);}
    if(key==GLFW_KEY_BACKSPACE && !a.typed.empty()) a.typed.pop_back();
    if(key==GLFW_KEY_ENTER || key==GLFW_KEY_KP_ENTER) {
        if(!a.typed.empty()) {
            try {
                const int newN=std::stoi(a.typed);
                if(newN>=1 && newN<=10000) { a.count=newN; a.layoutDirty=true; }
                else std::cout << "Count must be from 1 to 10000\n";
            } catch(...) { std::cout << "Invalid Bunny count\n"; }
            a.typed.clear();
        }
    }
}
void charCallback(GLFWwindow* w,unsigned int codepoint) {
    App& a=*static_cast<App*>(glfwGetWindowUserPointer(w));
    if(codepoint>='0' && codepoint<='9' && a.typed.size()<5)
        a.typed.push_back(static_cast<char>(codepoint));
}
void uploadMesh(App& a) {
    glGenVertexArrays(1,&a.vao);glBindVertexArray(a.vao);
    glGenBuffers(1,&a.vbo);glBindBuffer(GL_ARRAY_BUFFER,a.vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(BunnyMesh::vertices),BunnyMesh::vertices,GL_STATIC_DRAW);
    glGenBuffers(1,&a.ebo);glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,a.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(BunnyMesh::indices),BunnyMesh::indices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    // Optional visual debugging aid: circle = projection of enclosing sphere onto XY.
    const int steps=64;
    std::vector<float> circle;
    const double pi=3.14159265358979323846;
    for(int i=0;i<steps;++i) { const double angle=2*pi*i/steps;
        circle.push_back((float)std::cos(angle));circle.push_back((float)std::sin(angle));circle.push_back(0.0f); }
    glGenVertexArrays(1,&a.circleVao); glBindVertexArray(a.circleVao);
    glGenBuffers(1,&a.circleVbo);glBindBuffer(GL_ARRAY_BUFFER,a.circleVbo);
    glBufferData(GL_ARRAY_BUFFER,(GLsizeiptr)(circle.size()*sizeof(float)),circle.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}
}
int main() {
    App a;
    a.count=getInitialCount();
    try {
        const auto bounds=lab03::computeAABB(BunnyMesh::vertices,BunnyMesh::vertexCount);
#ifdef LAB03_USE_STUDENT_MATH
        a.sphere=lab03::student::computeBoundingSphere(BunnyMesh::vertices,BunnyMesh::vertexCount,bounds);
#else
        a.sphere=lab03::computeBoundingSphere(BunnyMesh::vertices,BunnyMesh::vertexCount,bounds);
#endif
        std::cout << "Mesh: " << BunnyMesh::vertexCount << " vertices, "
                  << BunnyMesh::indexCount/3 << " triangles\nSphere radius: " << a.sphere.radius << '\n';
        std::cout << "Controls: type ANY integer 1..10000, ENTER to apply; BACKSPACE to edit; "
                  << "R rotation; B debug sphere; W wireframe; V vsync; ESC exit.\n";
        if(!glfwInit()) throw std::runtime_error("GLFW initialization failed");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
        a.window=glfwCreateWindow(800,600,"Lab03 - Stanford Bunny Adaptive Grid",nullptr,nullptr);
        if(!a.window) throw std::runtime_error("Cannot create OpenGL window");
        glfwMakeContextCurrent(a.window);
        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("GLAD initialization failed");

     

        

        std::cout << "GL_VENDOR: " << glGetString(GL_VENDOR) << "\nGL_RENDERER: " << glGetString(GL_RENDERER) << '\n';
        glfwSetWindowUserPointer(a.window,&a);
        glfwSetFramebufferSizeCallback(a.window,framebufferSize);
        glfwSetKeyCallback(a.window,keyCallback);
        glfwSetCharCallback(a.window,charCallback);

        // ===================================
// LAB 04 - Initialize Dear ImGui
// ===================================
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(a.window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        glfwGetFramebufferSize(a.window,&a.framebufferWidth,&a.framebufferHeight);
        glViewport(0,0,a.framebufferWidth,a.framebufferHeight);
        glfwSwapInterval(0);
        glEnable(GL_DEPTH_TEST);
        a.program=shaderProgram();uploadMesh(a);
        updateLayout(a);
        double last=glfwGetTime(), fpsLast=last, fps=0;int frames=0;
        double animationTime=0;
        while(!glfwWindowShouldClose(a.window)) {
            glfwPollEvents();
            int fbw=0,fbh=0;glfwGetFramebufferSize(a.window,&fbw,&fbh);
            if(fbw==0 || fbh==0){glfwWaitEvents();continue;}
            if(a.layoutDirty) updateLayout(a); // once on resize or count change, NOT per rotation frame
            const double now=glfwGetTime(), dt=std::min(.1,now-last); last=now;
            if(a.rotate) animationTime+=dt;
            glClearColor(.045f,.055f,.08f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            glUseProgram(a.program);
            glUniform3fv(
                glGetUniformLocation(a.program, "uSolidColor"),
                1,
                solidColor
            );
            glUniform1i(
                glGetUniformLocation(a.program, "uRenderMode"),
                renderMode
            );
            glUniform3fv(
                glGetUniformLocation(a.program, "uCheckerColorA"),
                1,
                checkerColorA
            );

            glUniform3fv(
                glGetUniformLocation(a.program, "uCheckerColorB"),
                1,
                checkerColorB
            );

            glUniform1f(
                glGetUniformLocation(a.program, "uCheckerFrequency"),
                checkerFrequency
            );
            
            glUniform1f(
                glGetUniformLocation(a.program, "uTime"),
                static_cast<float>(glfwGetTime())
            );

            glUniform1f(
                glGetUniformLocation(a.program, "uPulseSpeed"),
                pulseSpeed
            );

            glUniform1f(
                glGetUniformLocation(a.program, "uGlowIntensity"),
                glowIntensity
            );
            glUniform3fv(
                glGetUniformLocation(a.program, "uFireflyDark"),
                1,
                fireflyDark
            );

            glUniform3fv(
                glGetUniformLocation(a.program, "uFireflyBright"),
                1,
                fireflyBright
            );
            glUniform3f(glGetUniformLocation(a.program,"uMeshCenter"),
                        (float)a.sphere.center.x,(float)a.sphere.center.y,(float)a.sphere.center.z);
            glUniform1i(glGetUniformLocation(a.program,"uColumns"),a.grid.columns);
            glUniform2f(glGetUniformLocation(a.program,"uCellSize"),
                        (float)a.grid.cellWidth,(float)a.grid.cellHeight);
            glUniform2f(glGetUniformLocation(a.program,"uWorldSize"),
                        (float)a.grid.worldWidth,(float)a.grid.worldHeight);
            glUniform1f(glGetUniformLocation(a.program,"uFixedScale"),(float)a.grid.scale);
            glUniform1f(glGetUniformLocation(a.program,"uTime"),(float)animationTime);
            glUniform1f(glGetUniformLocation(a.program,"uRadius"),(float)a.sphere.radius);
            glUniform1i(glGetUniformLocation(a.program,"uSphereOutline"),0);
            glPolygonMode(GL_FRONT_AND_BACK,a.wireframe?GL_LINE:GL_FILL);
            glBindVertexArray(a.vao);
            glDrawElementsInstanced(GL_TRIANGLES,(GLsizei)BunnyMesh::indexCount,
                                    GL_UNSIGNED_INT,nullptr,a.count);
            if(a.outlines && a.count<=1000) {
                glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
                glUniform1i(glGetUniformLocation(a.program,"uSphereOutline"),1);
                glBindVertexArray(a.circleVao);
                glDrawArraysInstanced(GL_LINE_LOOP,0,64,a.count);
            }
            glBindVertexArray(0);
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

            // LAB 04 - GUI frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::SetNextWindowSize(
                ImVec2(340, 220),
                ImGuiCond_FirstUseEver
            );
            ImGui::Begin("Lab 04 - Controls");
            ImGui::Separator();

           
            ImGui::Text("Stanford Bunny Graphics Lab");

            // LAB 04 - Triangle statistics
            constexpr int trianglesPerBunny = 4968;

            long long trianglesPerFrame =
                static_cast<long long>(a.count) * trianglesPerBunny;

            ImGui::Separator();
            ImGui::Text("Rendering Statistics");

            ImGui::Text("Bunnies: %d", a.count);
            ImGui::Text("Triangles per Bunny: %d", trianglesPerBunny);
            ImGui::Text("Triangles per Frame: %lld", trianglesPerFrame);

            ImGui::SetNextItemWidth(350.0f);

            bool changed = ImGui::SliderInt(
                "Number of Bunnies",
                &a.count,
                1,
                500
            );

            ImGui::SetNextItemWidth(120.0f);

            changed |= ImGui::InputInt(
                "Exact Bunny Count",
                &a.count
            );

            if (changed)
            {
                a.count = std::clamp(a.count, 1, 10000);
                a.layoutDirty = true;
            }
            

            ImGui::Separator();
            ImGui::Text("Fragment Shader Effects");

            ImGui::Combo(
                "Rendering Mode",
                &renderMode,
                "Solid Color\0Checkerboard\0Fireflies\0"
            );

            if (renderMode == 0)
            {
                ImGui::Separator();
                ImGui::Text("Solid Color Settings");

                ImGui::ColorEdit3("Bunny Color", solidColor);
            }

            // Only display these controls in Checkerboard mode
            if (renderMode == 1)
            {
                ImGui::Separator();
                ImGui::Text("Checkerboard Settings");

                ImGui::ColorEdit3("Color A", checkerColorA);
                ImGui::ColorEdit3("Color B", checkerColorB);

                ImGui::SliderFloat(
                    "Frequency",
                    &checkerFrequency,
                    1.0f,
                    40.0f
                );
            }
            if (renderMode == 2)
            {
                ImGui::Separator();
                ImGui::Text("Fireflies Settings");

                ImGui::SliderFloat(
                    "Pulse Speed (Hz)",
                    &pulseSpeed,
                    0.0f,
                    5.0f,"%.2f"
                );

                ImGui::SliderFloat(
                    "Glow Intensity",
                    &glowIntensity,
                    0.0f,
                    1.0f,"%.2f"
                );

                ImGui::ColorEdit3(
                    "Dark Color",
                    fireflyDark
                );

                ImGui::ColorEdit3(
                    "Bright Color",
                    fireflyBright
                );


                // Debug: show the actual C++ values
                ImGui::Text("Speed value: %.2f", pulseSpeed);
                ImGui::Text("Intensity value: %.2f", glowIntensity);
            }

            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(a.window);

            ++frames;
            if(now-fpsLast>=0.6) {
                fps=frames/(now-fpsLast);frames=0;fpsLast=now;
                std::ostringstream name;
                name << "Lab03 | N="<<a.count <<" | grid="<<a.grid.columns<<'x'<<a.grid.rows
                     <<" | FPS="<<std::fixed<<std::setprecision(1)<<fps
                     <<" ("<<std::setprecision(2)<<(fps>0?1000.0/fps:0.0)<<" ms)"
                     <<" | "<<(a.typed.empty()?"Type digits + Enter":("Pending count: "+a.typed))
                     <<" | R rotate | B sphere | W wireframe | V VSync";
                glfwSetWindowTitle(a.window,name.str().c_str());
            }
        }
        glDeleteProgram(a.program);
        glDeleteVertexArrays(1,&a.vao);glDeleteBuffers(1,&a.vbo);glDeleteBuffers(1,&a.ebo);
        glDeleteVertexArrays(1,&a.circleVao);glDeleteBuffers(1,&a.circleVbo);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(a.window);glfwTerminate();
        return 0;
    } catch(const std::exception& e) {
        std::cerr << "Lab03 error: " << e.what() << '\n';
        if(a.window) glfwDestroyWindow(a.window);
        glfwTerminate();return 1;
    }
}
