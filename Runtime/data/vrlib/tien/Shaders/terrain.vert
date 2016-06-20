#version 150

#include "common.glsl"

in vec3 a_position;
in vec3 a_normal;
in vec3 a_bitangent;
in vec3 a_tangent;
in vec2 a_texture;

out vec2 texCoord;
out vec4 position;

out vec3 normal;
out vec3 tangent;
out vec3 bitangent;

//out mat3 TBN;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;
out float height;

void main()
{
	texCoord = a_texture;

	normal = normalMatrix * (a_normal);
	bitangent = normalMatrix * (a_bitangent);
	tangent = normalMatrix * (a_tangent);

/*	TBN = mat3( tangent,
				bitangent,
				normal);*/

	height = a_position.y;

	position = vec4(a_position,1.0);

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(a_position,1.0);


}