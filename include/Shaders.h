#ifndef SHADERS_HPP
#define SHADERS_HPP

const char* vertexShaderSource = R"glsl(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)glsl";

const char* fragmentShaderSource = R"glsl(#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture1;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform bool isGlowing;

void main() {
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse);
    
    if(isGlowing) {
        result += vec3(0.8, 0.8, 1.0);
    }

    FragColor = texture(texture1, TexCoord) * vec4(result, 1.0);
}
)glsl";

#endif