#version 150

uniform sampler2D s_texture;
in vec2 texCoord;
in vec3 normal;
uniform vec4 colorMult;

void main()
{
	
	float diffuse = dot(normalize(vec3(1,1,1)), normalize(normal));
	diffuse = clamp(diffuse, 0, 1);

	float light = 0.5 * diffuse + 0.5;


	vec4 tex = texture2D(s_texture, texCoord);

	gl_FragColor.rgb = light * tex.rgb * colorMult.rgb;
	gl_FragColor.a = tex.a * colorMult.a;
	//gl_FragColor.rgb = vec3(1,0,0);
}
