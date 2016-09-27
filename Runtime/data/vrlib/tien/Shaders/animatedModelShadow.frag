#version 330 core
layout(location = 0) out float fragmentdepth;

in vec4 position;
uniform bool outputPosition;

void main()
{
	if(outputPosition)
		fragmentdepth = length(position.xyz);
	else
		fragmentdepth = gl_FragCoord.z;
}
