#version 150

uniform sampler2D s_texture;
uniform vec4 color;
uniform float textureFactor;
uniform bool showAlpha;

in vec2 texcoord;
out vec4 fragColor;

void main()
{
	if(showAlpha)
	{
		float alpha = texture2D( s_texture, texcoord ).a;
		fragColor = vec4(alpha,alpha,alpha,1);
	}
	else
		fragColor = mix(color, texture2D( s_texture, texcoord ), textureFactor);
}
