varying vec3 lightDir,normal;

varying float canDiscard;
uniform vec3 cutNormal;
uniform vec3 cutPos;
uniform mat4 modelMatrix;

varying vec3 position;

void main()
{
    normal = normalize(gl_NormalMatrix * gl_Normal);
 
    lightDir = normalize(vec3(0,1,0));
    gl_TexCoord[0] = gl_MultiTexCoord0;
 
	position = vec3(modelMatrix * vec4(vec3(gl_Vertex),1));

    gl_Position = ftransform();
}