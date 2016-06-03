#version 150

in vec3 a_position;


uniform int lightType;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrixInv;
uniform mat4 modelViewMatrixInv;


void main()
{
	switch(lightType)
	{
		case 0:
			gl_Position = vec4(a_position, 1.0);
			break;
		case 1:
		case 2:
			gl_Position = projectionMatrix * modelViewMatrix * vec4(a_position, 1.0);
			break;
	}
}