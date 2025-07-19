#include "Wheel.h" 
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>


Wheel::Wheel(int wConfig)
    : position(0.0f, 0.0f, 0.0f),
      color(0.5f, 0.5f, 0.0f), // Default black color
      scale(1.0f, 1.0f, 1.0f)
{
    wheelConfig = wConfig;
    updateModelMatrix();
}

Wheel::~Wheel() {
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

void Wheel::updateModelMatrix() {
    modelMatrix[3] = glm::vec4(position, 1.0f);
    // Add rotation (e.g., based on velocity direction or explicit rotation variables)
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), rotationAxis);
    modelMatrix = glm::scale(modelMatrix, scale);
}

void Wheel::setupGPUBuffers() {
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
}

bool Wheel::loadModel() {
    const char* modelPath;
    if(wheelConfig==LEFTWHEEL) modelPath = "assets/F1_car/C44/seperated_left_break.obj";
    else modelPath = "assets/F1_car/C44/seperated_right_break.obj";
    std::ifstream file(modelPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open OBJ file: " << modelPath << std::endl;
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

    return true;
}


void Wheel::draw(Shader& carshader) {
    if (VAO == 0) {
        std::cerr << "Warning: Car VAO is not set up. Call setupGPUBuffers() first." << std::endl;
        return;
    }

    // Use the shader program
    // carshader.use();

    // Pass the model matrix to the shader
    // carshader.setMat4("model", modelMatrix);
    carshader.setVec3("objectColor", color);

    // Bind the VAO and draw
    glBindVertexArray(VAO);
    if (!indices.empty()) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    } else {
        // If no indices, assume a simple array of vertices (e.g., triangle list)
        glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // Assuming 3 floats per vertex position
    }
    glBindVertexArray(0); // Unbind VAO

    // glUseProgram(0); // Unuse shader program.
}