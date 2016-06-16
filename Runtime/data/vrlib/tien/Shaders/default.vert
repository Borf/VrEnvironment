#version 150

in vec3 a_position;
in vec3 a_normal;
in vec3 a_tangent;
in vec2 a_texture;

out vec2 texCoord;
out vec4 position;

out vec3 normal;
out vec3 tangent;
out vec3 bitangent;

out mat3 TBN;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;


void main()
{
	texCoord = a_texture;


	normal = normalMatrix * a_normal;
	tangent = normalMatrix * a_tangent;
	bitangent = normalize(cross(a_normal, a_tangent));
	


//	TBN = mat3(	normal), 
//				vec3(modelMatrix * vec4(bitangent,0)), 
//				vec3(modelMatrix * vec4(a_normal,0)));
	TBN = mat3( normalMatrix * a_tangent,
				normalMatrix * bitangent,
				normalMatrix * a_normal);


	position = modelMatrix * vec4(a_position,1.0);

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(a_position,1.0);


}