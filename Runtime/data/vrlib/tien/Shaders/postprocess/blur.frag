#version 150
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D s_color;
uniform sampler2D s_normal;
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
	vec4 color = vec4(0,0,0,0);
	
	if(direction == 0)
	{
		/*color += 1.0 / 16.0 * texture2DOffset(s_color, texCoord, ivec2(-2,0));
		color += 1.0 /  4.0 * texture2DOffset(s_color, texCoord, ivec2(-1,0));
		color += 3.0 /  8.0 * texture2DOffset(s_color, texCoord, ivec2(0,0));
		color += 1.0 /  4.0 * texture2DOffset(s_color, texCoord, ivec2(1,0));
		color += 1.0 / 16.0 * texture2DOffset(s_color, texCoord, ivec2(2,0));*/

		color += strength * 1.0 / 64.0 * texture2DOffset(s_color, texCoord, ivec2(-3,0));
		color += strength * 6.0 / 64.0 * texture2DOffset(s_color, texCoord, ivec2(-2,0));
		color += strength * 15.0 / 64.0 * texture2DOffset(s_color, texCoord, ivec2(-1,0));
		color += (1 + strength * 20.0 / 64.0) * texture2DOffset(s_color, texCoord, ivec2(0,0));
		color += strength * 15.0 / 64.0 * texture2DOffset(s_color, texCoord, ivec2(1,0));
		color += strength * 6.0 / 64.0 * texture2DOffset(s_color, texCoord, ivec2(2,0));
		color += strength * 1.0 / 64.0 * texture2DOffset(s_color, texCoord, ivec2(3,0));

	}
	else
	{
		/*color += 1.0 / 16.0 * texture2DOffset(s_color, texCoord, ivec2(0, -2));
		color += 1.0 /  4.0 * texture2DOffset(s_color, texCoord, ivec2(0, -1));
		color += 3.0 /  8.0 * texture2DOffset(s_color, texCoord, ivec2(0,0));
		color += 1.0 /  4.0 * texture2DOffset(s_color, texCoord, ivec2(0, 1));
		color += 1.0 / 16.0 * texture2DOffset(s_color, texCoord, ivec2(0, 2));*/

		color += strength * 1.0 / 64.0 * texture2DOffset(s_color, texCoord, ivec2(0,-3));
		color += strength * 6.0 / 64.0 * texture2DOffset(s_color, texCoord, ivec2(0,-2));
		color += strength * 15.0 / 64.0 * texture2DOffset(s_color, texCoord, ivec2(0,-1));
		color += (1 + strength * 20.0 / 64.0) * texture2DOffset(s_color, texCoord, ivec2(0,0));
		color += strength * 15.0 / 64.0 * texture2DOffset(s_color, texCoord, ivec2(0,1));
		color += strength * 6.0 / 64.0 * texture2DOffset(s_color, texCoord, ivec2(0,2));
		color += strength * 1.0 / 64.0 * texture2DOffset(s_color, texCoord, ivec2(0,3));

	}

	fragColor = color;
}
