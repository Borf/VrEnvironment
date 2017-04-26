#version 150

uniform sampler2D s_color;
uniform sampler2D s_normal;
uniform sampler2D s_depth;


in vec2 texCoord;
out vec4 fragColor;

void main()
{
	vec4 color = vec4(0,0,0,0);
	
	float size = 0.001;
	
	color += 0.111 * texture2D( s_color, texCoord + vec2(-size, -size) ); 
	color += 0.111 * texture2D( s_color, texCoord + vec2( 0.0,  -size) ); 
	color += 0.111 * texture2D( s_color, texCoord + vec2( size, -size) ); 

	color += 0.111 * texture2D( s_color, texCoord + vec2(-size,  0.0) ); 
	color += 1.111 * texture2D( s_color, texCoord + vec2( 0.0,   0.0) ); 
	color += 0.111 * texture2D( s_color, texCoord + vec2( size,  0.0) ); 

	color += 0.111 * texture2D( s_color, texCoord + vec2(-size,  size) ); 
	color += 0.111 * texture2D( s_color, texCoord + vec2( 0.0,   size) ); 
	color += 0.111 * texture2D( s_color, texCoord + vec2( size,  size) ); 

	fragColor = color;
}
