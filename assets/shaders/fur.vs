#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 texCoordVs;
out vec3 normal;

void main()
{
    texCoordVs = a_texcoord;

    mat3 normalMatrixCalc = transpose(inverse(mat3(viewMatrix * modelMatrix)));
    normal = normalMatrixCalc * a_normal;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(a_position, 1.0);
}