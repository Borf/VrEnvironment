#version 150

uniform sampler2D s_texture;
uniform vec4 diffuseColor;
uniform float textureFactor;

in vec2 texCoord;
in vec3 normal;
in vec3 debugColor;
out vec4 fragColor;

void main()
{
	
	float diffuse = dot(normalize(vec3(1,1,1)), normalize(normal));
	diffuse = clamp(diffuse, 0, 1);

	float light = 0.5 * diffuse + 0.5;


	vec4 tex = mix(diffuseColor, texture2D(s_texture, texCoord), textureFactor);
	if(tex.a < 0.01)
		discard;
	fragColor.rgb = light * tex.rgb;// * debugColor;
	fragColor.a = tex.a;
}
