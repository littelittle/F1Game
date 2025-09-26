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