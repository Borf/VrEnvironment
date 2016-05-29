#version 150

uniform sampler2D s_texture;
uniform vec4 diffuseColor;
uniform float textureFactor;

in vec2 texCoord;
in vec3 normal;
in vec4 position;
out vec4 fragColor;
out vec4 fragNormal;
out vec4 fragPosition;

void main()
{
	vec4 tex = mix(diffuseColor, texture2D(s_texture, texCoord), textureFactor);
	if(tex.a < 0.01)
		discard;
	fragColor.rgb = tex.rgb;
	fragColor.a = tex.a;

	fragNormal = vec4(normal,1);
	fragPosition = position;
	fragPosition.a = 1;
}
