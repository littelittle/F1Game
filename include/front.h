#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <Shader.h>
#include <Wheel.h>

class Car;

#define LEFTWHEEL 1
#define RIGHTWHEEL 2
#define MAXSTEERINGANGLE 30.0f


class Front {
public:
    Front(int wheelConfig, const Car& car);
    ~Front();
    void draw(Shader& carshader);
    void update(float deltaTime);
 
    // Setters
    void setPosition(const glm::vec3& newPosition);
    void setVelocity(const glm::vec3& newVelocity);
    void setColor(const glm::vec3& newColor);
    void setTexture(const std::string& texturePath); // Will load texture and set textureID
    void setScale(const glm::vec3& newScale); // Optional, for scaling the model

    // Controls
    void updateAcceleration(const glm::vec3& deltaAcceleration);
    void addBreak(bool);
    void turnLeft(bool);
    void turnRight(bool);

    // Getters (const-correct for safety)
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getColor() const { return color; }
    glm::mat4 getModelMatrix() const { return modelMatrix; }


    // Model loading and GPU buffer setup
    bool loadModel(); // Returns true on success
    void setupGPUBuffers(); // Pushes loaded vertices/indices to GPU

private:
    const Car& car;

    Wheel wheel; 

    glm::vec3 position;
    glm::vec3 color;        // Added
    glm::vec3 scale;        // Optional, added
    glm::vec3 shift;        // the shift from the center of the car
    
    int wheelConfig;
    float breakTime;
    bool breakStatus;
    float turning;
    float angle;

    // Model Data
    std::vector<glm::vec3> vertices; 
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    // OpenGL handles
    GLuint VAO;
    GLuint vertexVBO, uvVBO, normalVBO;
    GLuint EBO;
    GLuint textureID; // Added for texture

    // Transformation Matrix
    glm::mat4 modelMatrix; // Added
    glm::mat4 steeringMatrix;
    glm::mat4 steeringMatrixX;
    void updateModelMatrix(); // Helper to update the modelMatrix based on position, rotation, scale
    void rotateModelMatrixAroundY_Simplified(float angleDegree);

    friend class Car;
};