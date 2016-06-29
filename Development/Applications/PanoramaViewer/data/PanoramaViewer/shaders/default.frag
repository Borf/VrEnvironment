#version 150

uniform sampler2D s_texture;
uniform vec4 diffuseColor;
uniform float textureFactor;

in vec2 texCoord;
in vec3 normal;
out vec4 fragColor;
in vec4 position;

void main()
{

	vec2 texcoord;
	texcoord.y = 1 - position.y/20.0+0.5;
	texcoord.x = atan(position.x/10.0, position.z/10.0) / (2 * 3.1415);
	
	vec4 tex = texture2D(s_texture, texcoord);

	fragColor.rgb = tex.rgb;
	fragColor.a = tex.a;
}
