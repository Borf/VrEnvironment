#version 150

in vec3 a_position;
in vec2 a_texture;

out vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

void main()
{
	texCoord = a_texture;
	gl_Position = projectionMatrix * modelMatrix * vec4(a_position,1.0);
}