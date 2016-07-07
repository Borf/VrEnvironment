#version 150

uniform sampler2D s_texture;
uniform vec4 diffuseColor;
uniform float textureFactor;
uniform bool useSphereMap;
uniform float offset;

in vec2 texCoord;
in vec3 normal;
in vec4 position;

out vec4 fragColor;

void main()
{

	if(useSphereMap)
	{
		vec2 texcoord;
		texcoord.y = 1 - acos(position.y) / 3.14159265;
		texcoord.x = 1 - (atan(position.x, position.z) + 3.14159265) / (2 * 3.14159265) + offset;
	
		fragColor.rgb = texture2D(s_texture, texcoord).rgb;
	}
	else
		fragColor.rgb = texture2D(s_texture, texCoord).rgb;
	fragColor.a = 1;
}
