#pragma once
#include <GL/glew.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern const char* vertexShaderSource;

extern const char* fragmentShaderSource;

GLuint compileShader(const char* shaderSource, GLenum shaderType);

// GLuint createShaderProgram();

GLuint createShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath);

class Shader {
    public:
        GLuint ID; // Program ID
    
        // Shader() {
        //     ID = createShaderProgram();
        //     if (ID == 0) {
        //         std::cerr << "Failed to create shader program." << std::endl;
        //     }
        // }

        Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
            ID = createShaderProgram(vertexShaderPath, fragmentShaderPath);
            if (ID == 0) {
                std::cerr << "Failed to create shader program." << std::endl;
            }

        }
    
        // 激活 Shader
        void use() {
            glUseProgram(ID);
        }
    
        // 设置 uniform 变量的辅助函数
        void setBool(const std::string& name, bool value) const {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        }
        void setInt(const std::string& name, int value) const {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        }
        void setFloat(const std::string& name, float value) const {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        }
        void setVec3(const std::string& name, float x, float y, float z) const {
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
        }
        void setVec3(const std::string& name, const glm::vec3& value) const {
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setMat4(const std::string& name, const glm::mat4& mat) const {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
    
        // 析构函数，清理资源
        ~Shader() {
            if (ID != 0) {
                glDeleteProgram(ID);
            }
        }
    };