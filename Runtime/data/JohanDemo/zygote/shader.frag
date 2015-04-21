varying vec3 lightDir,normal;
uniform sampler2D tex;
uniform int hasTexture;
uniform vec4 colorMult;
uniform vec3 cutNormal;
uniform vec3 cutPos;
varying vec3 position;

void main()
{

	vec3 p = position - cutPos;
	float canDiscard = dot(p, cutNormal);

	if(canDiscard < 0.0)
		discard;

    float intensity = max(dot(lightDir,normalize(normal)),0.5);
 
    vec4 texel;
	if(hasTexture == 1)
		texel = colorMult * texture2D(tex,gl_TexCoord[0].st);
	else
		texel = colorMult;
 
    gl_FragColor = vec4(intensity * texel.rgb, texel.a);
 
}