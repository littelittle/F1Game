#include "shader.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


GLuint compileShader(const char* shaderSource, GLenum shaderType) {

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // 检查编译错误
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(shader); // 删除失败的 shader
        return 0;
    }
    return shader;
}

std::string readToString(const char* path){
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()){
        std::cout << path << " cannot be read!" << std::endl;
        return {};
    }

    long size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::string str(size, '\0');
    file.read(&str[0], size);
    return str;
}

GLuint createShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath) {

    // 1. Read the shader code from specific path
    std::string vertexShaderSource = readToString(vertexShaderPath);
    std::string fragmentShaderSource = readToString(fragmentShaderPath);

    // 2. 编译 Shader
    GLuint vertexShader = compileShader(vertexShaderSource.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);

    if (vertexShader == 0 || fragmentShader == 0) {
        return 0; // 编译失败
    }

    // 3. 链接 Shader 到 Program
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    // 检查链接错误
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(programID); // 删除失败的 program
        programID = 0;
    }

    // 4. 删除不再需要的 Shader 对象 (它们已经被链接到 Program 中了)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programID;
}
