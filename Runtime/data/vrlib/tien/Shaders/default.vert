#version 150

in vec3 a_position;
in vec3 a_normal;
in vec3 a_tangent;
in vec2 a_texture;

out vec2 texCoord;
out vec4 position;

out mat3 TBN;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;


void main()
{
	texCoord = a_texture;

	vec3 bitangent = cross(a_normal, a_tangent);
	TBN = mat3(	vec3(modelMatrix * vec4(a_tangent,0)), 
				vec3(modelMatrix * vec4(bitangent,0)), 
				vec3(modelMatrix * vec4(a_normal,0)));
	position = modelMatrix * vec4(a_position,1.0);

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(a_position,1.0);


}