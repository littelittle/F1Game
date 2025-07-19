#include "Car.h" 
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

// Constructor: Initialize member variables
Car::Car()
    : position(0.0f, 0.0f, 0.0f),
      velocity(0.0f, 0.0f, 0.0f),
      acceleration(0.0f, 0.0f, 0.0f),
      color(1.0f, 0.0f, 0.0f), // Default red color
      scale(1.0f, 1.0f, 1.0f),
      breakTime(0.0f),
      angle(0.0f),
      leftWheel(LEFTWHEEL), rightWheel(RIGHTWHEEL),
      VAO(0), vertexVBO(0), uvVBO(0), normalVBO(0), textureID(0)
{
    modelMatrix = glm::mat4(1.0f);
    updateModelMatrix(); // Initialize model matrix
}

// Destructor: Clean up OpenGL resources
Car::~Car() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (vertexVBO != 0) {
        glDeleteBuffers(1, &vertexVBO);
    }
    if (uvVBO != 0){
        glDeleteBuffers(1, &uvVBO);
    }
    if (normalVBO != 0){
        glDeleteBuffers(1, &normalVBO);
    }
    // If you manage textures within the class, delete it here
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}

// Update the car's state based on time
void Car::update(float deltaTime) {
    // Basic Euler integration for physics
    if (breakStatus ) {
        if (glm::length(acceleration)*deltaTime > glm::length(velocity)){
            // Make sure not go backward
            std::cout << "reset to 0!!!" << std::endl;
            acceleration = {0, 0, 0};
            velocity = {0, 0, 0};
        }
        else velocity += acceleration * deltaTime;
    }
    else velocity += glm::mat3(modelMatrix) * acceleration * deltaTime;

    position += velocity * deltaTime;

    updateModelMatrix(); // Update model matrix after position changes
}

// Draw the car
void Car::draw(Shader& carshader) {
    if (VAO == 0) {
        std::cerr << "Warning: Car VAO is not set up. Call setupGPUBuffers() first." << std::endl;
        return;
    }

    // Use the shader program
    // carshader.use();

    // Pass the model matrix to the shader
    carshader.setMat4("model", modelMatrix);
    carshader.setVec3("objectColor", color);

    // for (int i = 0; i < 4; ++i) {
    //     for (int j = 0; j < 4; ++j) {
    //         std::cout << modelMatrix[j][i] << "\t";
    //     }
    //     std::cout << std::endl;
    // }

    // Bind the VAO and draw
    glBindVertexArray(VAO);
    if (!indices.empty()) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    } else {
        // If no indices, assume a simple array of vertices (e.g., triangle list)
        glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // Assuming 3 floats per vertex position
    }
    glBindVertexArray(0); // Unbind VAO

    leftWheel.draw(carshader);
    rightWheel.draw(carshader);
    // glUseProgram(0); // Unuse shader program.
}

// Setter methods
void Car::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
    updateModelMatrix();
}

void Car::setVelocity(const glm::vec3& newVelocity) {
    velocity = newVelocity;
}

void Car::updateAcceleration(const glm::vec3& deltaAcceleration) {
    acceleration += deltaAcceleration;
    // std::cout << "updating Acceleration to " << acceleration[0] << std::endl; 
}

void Car::addBreak(bool status){
    breakStatus = status;
    if (status){
        if (glm::length(velocity)){
            acceleration = -5.0f * velocity/glm::length(velocity);
        }
    }
    else{
        acceleration = {0, 0, 0};
    }
}

void Car::rotateModelMatrixAroundY_Simplified(float angleDegree) {
    // glm::rotate(matrix, angle_radians, axis_vector)
    // 直接对 modelMatrix 进行旋转操作
    // 角度转换为弧度

    // modelMatrix = glm::rotate(modelMatrix, glm::radians(angleDegree), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegree), glm::vec3(0.0f, 1.0f, 0.0f));

    modelMatrix = modelMatrix * rotationMatrix; 

    velocity = glm::mat3(modelMatrix) * glm::length(velocity) * glm::vec3({1,0,0});
}

void Car::turnLeft(bool status){
    if (status){
        // std::cout << "left!" << modelMatrix[0][0] << std::endl;
        rotateModelMatrixAroundY_Simplified(5);
        // std::cout << "finish!" << modelMatrix[0][0] << std::endl;
    }
}

void Car::turnRight(bool status){
    if (status){
        rotateModelMatrixAroundY_Simplified(-5);
    }
}


void Car::setColor(const glm::vec3& newColor) {
    color = newColor;
}

void Car::setScale(const glm::vec3& newScale) {
    scale = newScale;
    updateModelMatrix();
}

bool Car::loadModel() {
    const char* mainModelPath = "assets/F1_car/C44/seperated_mainbody.obj";
    std::ifstream file(mainModelPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open OBJ file: " << mainModelPath << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            iss >> vertex.z >> vertex.y >> vertex.x;
            temp_positions.push_back(vertex);
        }
        else if (prefix == "vt"){
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (prefix == "vn"){
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f") {
            std::string vertex_str;
            int vertexIndex[3], uvIndex[3], normalIndex[3];
            char slash;
            
            for (int i=0; i<3; i++){
                iss >> vertex_str;
                std::stringstream v_ss(vertex_str);

                if (vertex_str.find("//") != std::string::npos) {
                    // Format: v//vn
                    v_ss >> vertexIndex[i] >> slash >> slash >> normalIndex[i];
                    uvIndex[i] = 0; // No UV provided
                } else if (vertex_str.find("/") != std::string::npos) {
                    // Format: v/vt or v/vt/vn
                    v_ss >> vertexIndex[i] >> slash;
                    if (v_ss.peek() != '/') { // Check if there's a vt
                        v_ss >> uvIndex[i];
                    } else { // It's v/ /vn
                        uvIndex[i] = 0; // No UV provided
                    }
                    if (v_ss.peek() == '/') { // Check if there's a vn
                        v_ss >> slash >> normalIndex[i];
                    } else {
                        normalIndex[i] = 0; // No Normal provided
                    }
                } else {
                    // Format: v (only position)
                    v_ss >> vertexIndex[i];
                    uvIndex[i] = 0;
                    normalIndex[i] = 0;
                }

                // OBJ indices are 1-based, so subtract 1 for 0-based vector access
                // Add the actual data to our output vectors
                if (vertexIndex[i] > 0 && vertexIndex[i] <= temp_positions.size()) {
                    vertices.push_back(temp_positions[vertexIndex[i] - 1]);
                } else {
                    std::cerr << "Warning: Invalid vertex index in face: " << vertexIndex[i] << std::endl;
                    // Handle error: perhaps push a default/zeroed vertex
                    vertices.push_back({0,0,0});
                }

                if (uvIndex[i] > 0 && uvIndex[i] <= temp_uvs.size()) {
                    uvs.push_back(temp_uvs[uvIndex[i] - 1]);
                } else {
                    // If no UV is provided or invalid, push a default
                    uvs.push_back({0,0});
                }

                if (normalIndex[i] > 0 && normalIndex[i] <= temp_normals.size()) {
                    normals.push_back(temp_normals[normalIndex[i] - 1]);
                } else {
                    // If no normal is provided or invalid, push a default
                    normals.push_back({0,0,0});
                }
            }

        }
        // Add more parsing for "vt" (texture coordinates) and "vn" (normals)
        // and handle faces that reference them (e.g., f v/vt/vn)
    }
    file.close();
    std::cout << "OBJ file loaded. Vertices: " << vertices.size()
              << ", UVs: " << uvs.size()
              << ", Normals: " << normals.size() << std::endl;

    leftWheel.loadModel();
    rightWheel.loadModel();

    return true;
}

// Set up GPU buffers (VAO, VBOs)
void Car::setupGPUBuffers() {
    if (vertices.empty()) {
        std::cerr << "Error: No vertex data to set up GPU buffers. Load a model first." << std::endl;
        return;
    }

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO (for vertex)
    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // Generate and bond VBO for uv coordinates
    glGenBuffers(1, &uvVBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(1);

    // Generate and binf VBO for normal 
    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    std::cout << "GPU buffers for car model set up successfully." << std::endl;

    leftWheel.setupGPUBuffers();
    rightWheel.setupGPUBuffers();
}

// Dummy texture loading for demonstration (you'd use a real image loading library)
void Car::setTexture(const std::string& texturePath) {
    std::cout << "Loading texture from: " << texturePath << " (Dummy function, replace with SOIL, stb_image, etc.)" << std::endl;
    // For real applications, use a library like SOIL, stb_image.h, etc.
    // This is just a placeholder to show where GLuint textureID would be set.
    // Example:
    // textureID = loadTextureFromFile(texturePath); // Your actual texture loading function
    // if (textureID == 0) {
    //     std::cerr << "Failed to load texture: " << texturePath << std::endl;
    // }
    if (textureID == 0) { // Simulate generation if not already generated
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        // Set texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Dummy texture data (e.g., a simple white pixel for now)
        unsigned char data[] = { 255, 255, 255, 255 }; // RGBA white pixel
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        std::cout << "Dummy texture created for car." << std::endl;
    }
}

// Private helper to update the model matrix
void Car::updateModelMatrix() {
    modelMatrix[3] = glm::vec4(position, 1.0f);
    // Add rotation (e.g., based on velocity direction or explicit rotation variables)
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), rotationAxis);
    modelMatrix = glm::scale(modelMatrix, scale);
}