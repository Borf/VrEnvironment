#version 150

in vec2 a_position;


uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrixInv;
uniform mat4 modelViewMatrixInv;

out vec2 texCoord;

void main()
{
	texCoord = (a_position + vec2(1,1)) / 2.0;

	gl_Position = vec4(a_position,0.0, 1.0);
}