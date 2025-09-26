#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "shader.h" // 确保你包含了你的着色器类头文件

class Dashboard {
public:
    Dashboard(int windowWidth, int windowHeight);
    ~Dashboard();

    void render(float rpm);
    void setWindowSize(int width, int height);

private:
    void setupGauge();
    void setupNeedle();

    Shader shader;
    int windowWidth;
    int windowHeight;

    // RPM 表盘的 OpenGL 对象
    GLuint gaugeVAO, gaugeVBO;
    std::vector<glm::vec2> gaugeVertices;

    // RPM 指针的 OpenGL 对象
    GLuint needleVAO, needleVBO;
};