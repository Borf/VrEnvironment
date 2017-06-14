#version 150

in vec3 a_position;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec4 position;

void main()
{
	position = viewMatrix * modelMatrix * vec4(a_position,1.0);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(a_position,1.0);
}