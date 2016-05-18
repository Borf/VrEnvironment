#version 150

uniform sampler2D s_color;
uniform sampler2D s_normal;
uniform sampler2D s_position;

in vec2 texCoord;

out vec4 fragColor;

uniform vec3 lightPosition;
uniform vec4 lightColor;
uniform float lightRange;


void main()
{
	vec3 cameraPosition = vec3(0,1.5,-1);

    vec4 image = texture2D( s_color, texCoord );
    vec4 position = texture2D( s_position, texCoord );
    vec3 normal = texture2D( s_normal, texCoord ).xyz;
    
    vec3 lightDir = lightPosition - position.xyz ;

	float len = length(lightPosition - position.xyz);

    if(len > lightRange)
		discard;

	float distanceFac = pow(1 - (len / lightRange), 1.0);

    normal = normalize(normal);
    lightDir = normalize(lightDir);
    
    vec3 eyeDir = normalize(cameraPosition-position.xyz);
    vec3 vHalfVector = normalize(lightDir.xyz+eyeDir);
    
    fragColor = distanceFac * (lightColor * max(dot(normal,lightDir),0) * image);// + pow(max(dot(normal,vHalfVector),0.0), 100) * 1.5);
	fragColor.a = 1;
}
