#version 150

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texture;

out vec2 texCoord;
out vec3 normal;
out vec4 position;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;


void main()
{
	texCoord = a_texture;
	normal = normalMatrix * a_normal;
	position = modelMatrix * vec4(a_position,1.0);

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(a_position,1.0);


}