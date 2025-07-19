#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <Shader.h>

class Circuit {
public:
    Circuit();
    ~Circuit();

    void setupGPUBuffers();
    void draw(Shader& shader);

    void setPosition(const glm::vec3& pos);
    void setColor(const glm::vec3& col);
    void setScale(const glm::vec3& scale);

private:
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 scale;
    GLuint VAO, VBO;
    glm::mat4 modelMatrix;
    void updateModelMatrix();
}; 