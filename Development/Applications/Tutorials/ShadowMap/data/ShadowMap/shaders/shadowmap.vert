#version 330 core
 
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 a_position;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(a_position,1.0);
}