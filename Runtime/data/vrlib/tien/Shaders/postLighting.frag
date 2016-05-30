#version 330

#include "common.glsl"

uniform sampler2D s_color;
uniform sampler2D s_normal;
uniform sampler2D s_position;

in vec2 texCoord;

out vec4 fragColor;


uniform int lightType;
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform vec4 lightColor;
uniform float lightRange;


void main()
{
	vec3 cameraPosition = vec3(0,1.5,-1);

    vec4 image = texture2D( s_color, texCoord );
    vec4 position = texture2D( s_position, texCoord );
    vec3 normal = decodeNormal(texture2D( s_normal, texCoord ).xy);
    
	float diffuse = 0;
	float ambient = 0.1;
	float specular = 0;

	switch(lightType) // directional light
	{
		case 0: // directional light
			diffuse = max(0, dot(normalize(normal), normalize(vec3(1,1,1))));
			break;	
		case 1: // point light
			ambient = 0;
			vec3 lightDir = lightPosition - position.xyz;
			float len = length(lightDir);
//		    if(len > lightRange)
//				discard;
			float distanceFac = pow(1 - (len / lightRange), 1.0);
			diffuse = distanceFac * clamp(dot(normalize(normal), normalize(lightDir)), 0, 1);
			break;	
		case 2: // spotlight
		
			break;	
		default:
			fragColor = vec4(0,1,1,1);
			break;
	}





//	distanceFac = 1;
//	lightDir = vec3(1,1,1);

 //   normal = normalize(normal);
 //   lightDir = normalize(lightDir);
    
//    vec3 eyeDir = normalize(cameraPosition-position.xyz);
 //   vec3 vHalfVector = normalize(lightDir.xyz+eyeDir);
    
    //fragColor = distanceFac * (lightColor * max(dot(normal,lightDir),0.15) * image);// + pow(max(dot(normal,vHalfVector),0.0), 100) * 1.5);
	fragColor = lightColor * (diffuse + ambient + specular) * image;
	fragColor.a = 1;

//	fragColor.rgb = abs(normal);

}
