#version 150

uniform sampler2D s_texture;
uniform vec4 color;
uniform float textureFactor;
uniform bool showAlpha;
uniform bool linearize = false;

in vec2 texcoord;
out vec4 fragColor;

void main()
{
	if(linearize)
	{
		vec4 color = texture2D( s_texture, texcoord );

		float zNear = 0.1;
		float zFar = 10.0;		
		float depthSample = 2.0 * color.r - 1.0;
		float zLinear = 2.0 * zNear * zFar / (zFar + zNear - depthSample * (zFar - zNear));

		fragColor = vec4(zLinear, zLinear, zLinear, 1);
	}
	else if(showAlpha)
	{
		float alpha = texture2D( s_texture, texcoord ).a;
		fragColor = vec4(alpha,alpha,alpha,1);
	}
	else
	{
		fragColor = mix(color, texture2D( s_texture, texcoord ), textureFactor);
		fragColor.a = 1;
	}
}
