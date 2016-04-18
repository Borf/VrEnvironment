#version 150

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texture;
in ivec4 a_boneIds;
in vec4 a_boneWeights;

out vec2 texCoord;
out vec3 normal;
out vec3 debugColor;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 boneMatrices[50];


vec3 colors[8] = vec3[](
	vec3(1,1,1),
	vec3(0,0,0),
	vec3(0,0,1),
	vec3(0,1,0),
	vec3(0,1,1),
	vec3(1,0,0),
	vec3(1,0,1),
	vec3(1,1,0)
);


void main()
{
	mat3 normalMatrix = mat3(viewMatrix * modelMatrix);
	normalMatrix = transpose(inverse(normalMatrix));

	debugColor = colors[a_boneIds.x%8];

	texCoord = a_texture;
	normal = normalMatrix * a_normal;

	mat4 boneTransform = boneMatrices[a_boneIds[0]] * a_boneWeights[0];
	boneTransform += boneMatrices[a_boneIds[1]] * a_boneWeights[1];
	boneTransform += boneMatrices[a_boneIds[2]] * a_boneWeights[2];
	boneTransform += boneMatrices[a_boneIds[3]] * a_boneWeights[3];

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * boneTransform * vec4(a_position,1.0);
}