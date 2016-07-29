#version 150

#include "common.glsl"

in vec3 a_position;
in vec3 a_normal;
in vec3 a_bitangent;
in vec3 a_tangent;
in vec2 a_texture;

out vec2 texCoord;
out vec3 position;

out vec3 normal;
out vec3 tangent;
out vec3 bitangent;

//out mat3 TBN;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;

void main()
{
	gl_Position = vec4(a_position,1.0);
}