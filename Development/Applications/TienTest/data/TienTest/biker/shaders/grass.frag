#version 330

#include "common.glsl"


//in vec3 Color;
//in vec2 texCoord;
in vec2 tex_coord;

out vec4 fragColor;
out vec4 fragNormal;
out vec4 fragPosition;

uniform float time;
uniform sampler2D s_texture;

void main()
{
	fragColor = texture2D(s_texture,tex_coord);
	fragNormal = vec4(0,0,1,1);
}
