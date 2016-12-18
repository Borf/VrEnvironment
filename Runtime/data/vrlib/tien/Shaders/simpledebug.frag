#version 150

uniform sampler2D s_texture;
uniform vec4 color;
uniform float textureFactor;

in vec2 texcoord;
out vec4 fragColor;

void main()
{
	fragColor = mix(color, texture2D( s_texture, texcoord ), textureFactor);
}
