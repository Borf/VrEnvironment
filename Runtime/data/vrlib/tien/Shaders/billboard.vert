#version 150

in vec3 a_position;
in vec2 a_texcoord;

out vec2 texCoord;
out vec3 position;

uniform mat4 projectionMatrix;
uniform mat4 mat;

void main()
{
	mat4 ModelView = mat;
	/*// Column 0:
	ModelView[0][0] = 1;
	//ModelView[0][1] = 0;
	ModelView[0][2] = 0;
 
	// Column 1:
	ModelView[1][0] = 0;
	//ModelView[1][1] = 1;
	ModelView[1][2] = 0;
 
	// Column 2:
	ModelView[2][0] = 0;
	//ModelView[2][1] = 0;
	ModelView[2][2] = 1;*/


	texCoord = a_texcoord;
	gl_Position = projectionMatrix * ModelView * vec4(a_position,1.0);
}