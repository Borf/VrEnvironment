#version 150
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D s_texture;
uniform sampler2D s_depth;

uniform int direction;
uniform float strength = 0.1;

in vec2 texCoord;
out vec4 fragColor;

//1,4,6,4,1
//1/16, 1/4, 3/8, 1/4, 1/16


//1,	6,		15,	20,	15,	6,	1
//total 64

void main()
{
	vec4 color;
	float z_b = texture2D( s_depth, texCoord ).x;


	float fStrength = z_b - strength;

	float kernel[7] = float[7]( 1, 6, 15, 20, 15, 6, 1);
	kernel[3] += fStrength * 10;

	float total = kernel[0] + kernel[1] + kernel[2] + kernel[3] + kernel[4] + kernel[5] + kernel[6];
	kernel[0] /= total;
	kernel[1] /= total;
	kernel[2] /= total;
	kernel[3] /= total;
	kernel[4] /= total;
	kernel[5] /= total;
	kernel[6] /= total;



	if(direction == 0)
	{
		color += kernel[0] * texture2DOffset(s_texture, texCoord, ivec2(-3,0));
		color += kernel[1] * texture2DOffset(s_texture, texCoord, ivec2(-2,0));
		color += kernel[2] * texture2DOffset(s_texture, texCoord, ivec2(-1,0));
		color += kernel[3] * texture2DOffset(s_texture, texCoord, ivec2(0,0));
		color += kernel[4] * texture2DOffset(s_texture, texCoord, ivec2(1,0));
		color += kernel[5] * texture2DOffset(s_texture, texCoord, ivec2(2,0));
		color += kernel[6] * texture2DOffset(s_texture, texCoord, ivec2(3,0));
	}
	else
	{
		color += kernel[0] * texture2DOffset(s_texture, texCoord, ivec2(0,-3));
		color += kernel[1] * texture2DOffset(s_texture, texCoord, ivec2(0,-2));
		color += kernel[2] * texture2DOffset(s_texture, texCoord, ivec2(0,-1));
		color += kernel[3] * texture2DOffset(s_texture, texCoord, ivec2(0,0));
		color += kernel[4] * texture2DOffset(s_texture, texCoord, ivec2(0,1));
		color += kernel[5] * texture2DOffset(s_texture, texCoord, ivec2(0,2));
		color += kernel[6] * texture2DOffset(s_texture, texCoord, ivec2(0,3));
	}



	fragColor = color;
}
