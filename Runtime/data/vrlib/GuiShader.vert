#version 150

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texture;

out vec2 texCoord;
out vec3 normal;

uniform mat4 projectionmatrix;
uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;

void main()
{
	mat3 normalMatrix = mat3(cameraMatrix * modelMatrix);
	normalMatrix = transpose(inverse(normalMatrix));


	texCoord = a_texture;
	normal = normalMatrix * a_normal;
	gl_Position = projectionmatrix * cameraMatrix * modelMatrix * vec4(a_position,1.0);
}