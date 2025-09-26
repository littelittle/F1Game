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