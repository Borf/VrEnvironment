#version 150

uniform sampler2D s_texture;
uniform vec4 color;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
	vec4 tex = texture2D(s_texture, texCoord) * color;
	if(tex.a < 0.01)
		discard;
	fragColor = tex;
}
