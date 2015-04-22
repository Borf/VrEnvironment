attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texture;

varying vec2 texCoord;
varying vec3 normal;

uniform mat4 projectionmatrix;
uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;

void main()
{
	texCoord = a_texture;
	normal = a_normal;
	gl_Position = projectionmatrix * cameraMatrix * modelMatrix * vec4(a_position,1.0);
}