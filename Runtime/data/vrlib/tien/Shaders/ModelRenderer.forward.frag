#version 330

#include "common.glsl"


uniform sampler2D s_texture;
uniform sampler2D s_normalmap;
uniform sampler2D s_specularmap;
uniform vec4 diffuseColor;
uniform float textureFactor;
uniform float shinyness;


in vec2 texCoord;
//in mat3 TBN;
in vec3 normal;
in vec3 bitangent;
in vec3 tangent;

in vec4 position;
out vec4 fragColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;


void main()
{
	mat3 TBN = mat3(	tangent,
						bitangent,
						normal);

	vec3 n = normalize(2.0 * texture2D(s_normalmap, texCoord).rgb - 1);
	n = normalize(TBN * n);

	//todo: add forward lighting here......

	
	float diffuse = clamp(0.5 + clamp(dot(normalize(vec3(0,-1,0)), n), 0, 1), 0, 1);


	vec4 tex = mix(diffuseColor, texture2D(s_texture, texCoord), textureFactor);
	if(tex.a < 0.01)
		discard;
	fragColor.rgb = diffuse * tex.rgb;
	fragColor.a = tex.a;
}
