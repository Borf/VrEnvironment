#version 330

#include "common.glsl"


uniform sampler2D s_texture;
uniform sampler2D s_normalmap;
uniform sampler2D s_mask;

in vec2 texCoord;
//in mat3 TBN;
in vec3 normal;
in vec3 bitangent;
in vec3 tangent;

in vec4 position;
out vec4 fragColor;
out vec4 fragNormal;
out vec4 fragPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
in float height;

void main()
{
	mat3 TBN = mat3(	tangent,
						bitangent,
						normal);

	vec3 n = normal;//normalize(2.0 * texture2D(s_normalmap, texCoord).rgb - 1);
	//n = TBN * n;

	vec4 tex = texture2D(s_texture, texCoord);
	fragColor.rgb = tex.rgb;
	fragColor.a = texture2D(s_mask, position.xz/128.0).r;

	fragNormal.xyz = encodeNormal(normalize(n));
}
