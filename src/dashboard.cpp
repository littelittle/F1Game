#include "dashboard.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <dashboard.h>
#include <shader.h>

#ifndef M_PI
#define M_PI 3.1415926535
#endif

Dashboard::Dashboard(int width, int height) 
    : windowWidth(width), windowHeight(height), 
      shader("path/to/dashboard.vs", "path/to/dashboard.fs") // <<<--- 修改为你的着色器路径
{
    setupGauge();
    setupNeedle();
}

Dashboard::~Dashboard() {
    glDeleteVertexArrays(1, &gaugeVAO);
    glDeleteBuffers(1, &gaugeVBO);
    glDeleteVertexArrays(1, &needleVAO);
    glDeleteBuffers(1, &needleVBO);
}

void Dashboard::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

void Dashboard::setupGauge() {
    // 1. 生成表盘弧线的顶点
    float radius = 100.0f;
    int segments = 40;
    // 我们绘制一个从-135度到135度的弧线
    float startAngle = -135.0f * M_PI / 180.0f;
    float endAngle = 135.0f * M_PI / 180.0f;
    float angleStep = (endAngle - startAngle) / segments;

    for (int i = 0; i <= segments; ++i) {
        float angle = startAngle + i * angleStep;
        gaugeVertices.push_back(glm::vec2(radius * cos(angle), radius * sin(angle)));
    }

    // 2. 创建 VBO 和 VAO
    glGenVertexArrays(1, &gaugeVAO);
    glGenBuffers(1, &gaugeVBO);

    glBindVertexArray(gaugeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gaugeVBO);
    glBufferData(GL_ARRAY_BUFFER, gaugeVertices.size() * sizeof(glm::vec2), &gaugeVertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Dashboard::setupNeedle() {
    // 指针就是一条线，从中心点到边缘
    float length = 95.0f;
    glm::vec2 needleVertices[] = {
        glm::vec2(0.0f, 0.0f),       // 中心点
        glm::vec2(0.0f, length)      // 指针尖端 (初始朝上)
    };

    glGenVertexArrays(1, &needleVAO);
    glGenBuffers(1, &needleVBO);

    glBindVertexArray(needleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, needleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(needleVertices), needleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Dashboard::render(float rpm) {
    // --- 准备工作 ---
    // 禁用深度测试，这样2D仪表盘会绘制在3D场景之上
    glDisable(GL_DEPTH_TEST);
    // 激活我们的2D着色器
    shader.use();

    // --- 设置投影和视图 ---
    // 创建一个正交投影矩阵，将屏幕坐标系 (0, width, height, 0) 映射到标准设备坐标
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f);
    shader.setMat4("projection", projection);

    // --- 绘制表盘背景弧线 ---
    glm::mat4 model = glm::mat4(1.0f);
    // 将表盘移动到左下角 (例如，在(120, 700)的位置)
    model = glm::translate(model, glm::vec3(120.0f, windowHeight - 120.0f, 0.0f));
    shader.setMat4("model", model);
    shader.setVec3("objectColor", 0.8f, 0.8f, 0.8f); // 灰色

    glLineWidth(3.0f); // 设置线宽
    glBindVertexArray(gaugeVAO);
    glDrawArrays(GL_LINE_STRIP, 0, gaugeVertices.size());

    // --- 绘制动态指针 ---
    // 计算旋转角度。假设最大RPM为8000，对应270度的范围 (-135 to 135)
    float maxRPM = 8000.0f;
    float zeroAngle = -135.0f; // 0 RPM 对应的角度
    float maxAngle = 135.0f;   // 8000 RPM 对应的角度
    
    float rpmRatio = glm::clamp(rpm / maxRPM, 0.0f, 1.0f);
    float needleAngleDegrees = zeroAngle + rpmRatio * (maxAngle - zeroAngle);

    // 指针模型默认是朝上的(90度)，所以我们需要先把它转到0度位置，再应用计算出的角度
    float initialRotation = -90.0f;
    
    // 创建一个新的model矩阵用于指针
    // 注意：这里的平移必须和表盘的平移完全一样，以确保它们在同一个中心点
    glm::mat4 needleModel = glm::mat4(1.0f);
    needleModel = glm::translate(needleModel, glm::vec3(120.0f, windowHeight - 120.0f, 0.0f));
    // 旋转指针
    needleModel = glm::rotate(needleModel, glm::radians(needleAngleDegrees + initialRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    
    shader.setMat4("model", needleModel);
    shader.setVec3("objectColor", 1.0f, 0.0f, 0.0f); // 红色

    glLineWidth(2.0f);
    glBindVertexArray(needleVAO);
    glDrawArrays(GL_LINES, 0, 2);

    // --- 清理状态 ---
    glLineWidth(1.0f); // 恢复默认线宽
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST); // 重新启用深度测试，为下一帧的3D渲染做准备
}