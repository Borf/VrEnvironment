#version 330

#include "common.glsl"


uniform sampler2D s_texture;
uniform sampler2D s_normalmap;
uniform vec4 diffuseColor;
uniform float textureFactor;

in vec2 texCoord;
in mat3 TBN;
in vec4 position;
out vec4 fragColor;
out vec4 fragNormal;
out vec4 fragPosition;



void main()
{

	vec3 n = normalize(TBN * normalize(2.0 * texture2D (s_normalmap, texCoord).rgb - 1.0).xyz);

	vec4 tex = mix(diffuseColor, texture2D(s_texture, texCoord), textureFactor);
	if(tex.a < 0.01)
		discard;
	fragColor.rgb = tex.rgb;
	fragColor.a = tex.a;

	fragNormal.xy = encodeNormal(normalize(n));
	//fragPosition = position;
}
