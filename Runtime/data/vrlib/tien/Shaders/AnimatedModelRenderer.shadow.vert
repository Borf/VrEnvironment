#version 150

in vec3 a_position;
in ivec4 a_boneIds;
in vec4 a_boneWeights;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 boneMatrices[50];

void main()
{
	mat4 boneTransform = boneMatrices[a_boneIds[0]] * a_boneWeights[0];
	boneTransform += boneMatrices[a_boneIds[1]] * a_boneWeights[1];
	boneTransform += boneMatrices[a_boneIds[2]] * a_boneWeights[2];
	boneTransform += boneMatrices[a_boneIds[3]] * a_boneWeights[3];

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * boneTransform * vec4(a_position,1.0);
}