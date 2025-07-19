#include "shader.h"
#include <GL/glew.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;     // 顶点位置属性
    layout (location = 1) in vec2 aTexCoord; // 纹理坐标属性
    layout (location = 2) in vec3 aNormal;  // 法线属性

    out vec2 TexCoord;  // 传递给片段着色器的纹理坐标
    out vec3 Normal;    // 传递给片段着色器的法线（世界空间）
    out vec3 FragPos;   // 传递给片段着色器的片段位置（世界空间）
    
    uniform mat4 model;       // 模型矩阵
    uniform mat4 view;        // 视图矩阵
    uniform mat4 projection;  // 投影矩阵
    
    void main()
    {
        // 计算顶点在裁剪空间中的最终位置
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        
        // 将纹理坐标直接传递给片段着色器（它会被插值）
        TexCoord = aTexCoord;

        // 将法线从模型空间转换到世界空间 (考虑非均匀缩放)
        Normal = mat3(transpose(inverse(model))) * aNormal;
        
        // 计算片段的世界空间位置
        FragPos = vec3(model * vec4(aPos, 1.0));
    }
)glsl";

extern const char* fragmentShaderSource = R"glsl(
    #version 330 core

    // 从顶点着色器接收的插值数据
    in vec2 TexCoord;  // 纹理坐标
    in vec3 Normal;    // 法线（世界空间）
    in vec3 FragPos;   // 片段位置（世界空间）

    out vec4 FragColor; // 片段的最终颜色输出

    // 其他可能需要的uniforms，例如光源位置、材质属性、纹理采样器等
    uniform vec3 objectColor;
    uniform vec3 lightColor;
    uniform vec3 lightPos;
    // uniform sampler2D ourTexture; // 如果有纹理

    void main()
    {
        // 示例：简单的漫反射光照
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // 示例：环境光
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;

        // 最终颜色 = 环境光 + 漫反射 (这里没有包含镜面光和纹理，你可以根据需要添加)
        // 如果有纹理，可能是 FragColor = vec4(ambient + diffuse, 1.0) * texture(ourTexture, TexCoord);
        FragColor = vec4(ambient + diffuse + objectColor, 1.0); 
    }
)glsl";

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

GLuint createShaderProgram() {

    // 2. 编译 Shader
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

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
