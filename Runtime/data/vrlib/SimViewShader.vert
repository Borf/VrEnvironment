#version 150

in vec3 a_position;
in vec3 a_normal;

out vec3 normal;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
	mat3 normalMatrix = mat3(viewMatrix);
	normalMatrix = transpose(inverse(normalMatrix));

	normal = normalMatrix * a_normal;
	gl_Position = projectionMatrix * viewMatrix * vec4(a_position,1.0);
}