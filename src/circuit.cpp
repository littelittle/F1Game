#include "circuit.h"
#include <vector>

Circuit::Circuit()
    : position(0.0f, -0.9f, 0.0f),
      color(0.2f, 0.7f, 0.2f), // Default green
      scale(100.0f, 1.0f, 100.0f),
      VAO(0), VBO(0)
{
    updateModelMatrix();
}

Circuit::~Circuit() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}

void Circuit::setPosition(const glm::vec3& pos) {
    position = pos;
    updateModelMatrix();
}

void Circuit::setColor(const glm::vec3& col) {
    color = col;
}

void Circuit::setScale(const glm::vec3& s) {
    scale = s;
    updateModelMatrix();
}

void Circuit::updateModelMatrix() {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
}

void Circuit::setupGPUBuffers() {
    // Square in XZ plane, Y=0
    // 2 triangles, 4 vertices
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 uv;
        glm::vec3 normal;
    };
    std::vector<Vertex> vertices = {
        //  pos                uv      normal
        { {-0.5f, 0.0f, -0.5f}, {0,0}, {0,1,0} },
        { { 0.5f, 0.0f, -0.5f}, {1,0}, {0,1,0} },
        { { 0.5f, 0.0f,  0.5f}, {1,1}, {0,1,0} },
        { {-0.5f, 0.0f,  0.5f}, {0,1}, {0,1,0} },
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };
    GLuint EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(1);
    // normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    glDeleteBuffers(1, &EBO); // EBO can be deleted if not needed after VAO setup
}

void Circuit::draw(Shader& shader) {
    shader.setMat4("model", modelMatrix);
    shader.setVec3("objectColor", color);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
} 