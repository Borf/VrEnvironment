#version 330

in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D s_texture;

void main()
{
	fragColor = texture2D(s_texture, texCoord);
}
