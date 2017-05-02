#version 150
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D s_texture;
uniform sampler2D s_depth;

uniform int direction;
uniform float focalDistance = 2;
uniform float focalDepth = 2;

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
	float zNear = 0.01f;
	float zFar = 500.0f;

    float z_n = 2.0 * z_b - 1.0;
    float z_e = 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));

	z_e = min(1, abs(z_e - focalDistance) / focalDepth);

	float fStrength = z_e;

	float kernel[7] = float[7](1/64.0, 6/64.0, 15/64.0, 20/64.0, 15/64.0, 6/64.0, 1/64.0);

	vec2 fac = vec2(0.002, 0.002);

	if(direction == 0)
	{
		color += kernel[0] * texture2D(s_texture, texCoord + fStrength * fac * vec2(-3,0));
		color += kernel[1] * texture2D(s_texture, texCoord + fStrength * fac * vec2(-2,0));
		color += kernel[2] * texture2D(s_texture, texCoord + fStrength * fac * vec2(-1,0));
		color += kernel[3] * texture2D(s_texture, texCoord + fStrength * fac * vec2(0,0));
		color += kernel[4] * texture2D(s_texture, texCoord + fStrength * fac * vec2(1,0));
		color += kernel[5] * texture2D(s_texture, texCoord + fStrength * fac * vec2(2,0));
		color += kernel[6] * texture2D(s_texture, texCoord + fStrength * fac * vec2(3,0));
	}
	else
	{
		color += kernel[0] * texture2D(s_texture, texCoord + fStrength * fac * vec2(0,-3));
		color += kernel[1] * texture2D(s_texture, texCoord + fStrength * fac * vec2(0,-2));
		color += kernel[2] * texture2D(s_texture, texCoord + fStrength * fac * vec2(0,-1));
		color += kernel[3] * texture2D(s_texture, texCoord + fStrength * fac * vec2(0,0));
		color += kernel[4] * texture2D(s_texture, texCoord + fStrength * fac * vec2(0,1));
		color += kernel[5] * texture2D(s_texture, texCoord + fStrength * fac * vec2(0,2));
		color += kernel[6] * texture2D(s_texture, texCoord + fStrength * fac * vec2(0,3));
	}

	//color = vec4(fStrength, fStrength, fStrength, 1);


	fragColor = color;
}
