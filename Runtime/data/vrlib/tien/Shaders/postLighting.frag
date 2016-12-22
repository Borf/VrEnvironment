#version 330

#include "common.glsl"

uniform sampler2D s_color;
uniform sampler2D s_normal;
uniform sampler2D s_depth;
uniform sampler2DShadow s_shadowmap;
uniform samplerCube s_shadowmapcube;

uniform mat4 projectionMatrix;
//uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrixInv;
uniform mat4 modelViewMatrixInv;
uniform mat4 shadowMatrix;

out vec4 fragColor;

uniform int lightType;
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform vec4 lightColor;
uniform float lightRange;
uniform vec2 windowSize = vec2(1024,1024);
uniform vec2 windowPos = vec2(0,0);
uniform bool lightCastShadow;
uniform vec3 cameraPosition;


vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);
mat4 biasMatrix = mat4(
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0
);

float insideBox(vec2 v, vec2 bottomLeft, vec2 topRight) {
    vec2 s = step(bottomLeft, v) - step(topRight, v);
    return s.x * s.y;   
}

void main()
{
	vec2 texCoord = (gl_FragCoord.xy - windowPos.xy) / windowSize;

    vec4 image = texture2D( s_color, texCoord );
    float depth = texture2D( s_depth, texCoord ).x;
    vec3 normal = normalize(decodeNormal(texture2D( s_normal, texCoord ).xyz));

	vec4 viewPos = vec4(texCoord.xy*2.0-1.0, depth*2.0-1.0, 1);
	vec4 tempPos = modelViewMatrixInv * projectionMatrixInv * viewPos;
	vec3 position = tempPos.xyz / tempPos.w;
    
	float diffuse = 0;
	float ambient = 0;
	float specular = 0;
	float visibility = 1.0;


	//image.rgb = normal.rgb;

	switch(lightType) // directional light
	{
		case 0: // directional light
			vec3 n = normalize( normal );

			diffuse = max(0, dot(n, normalize(lightPosition.xyz))) * 0.5;
			ambient = 0.5;

/*			// Eye vector (towards the camera)
			vec3 E = normalize(EyeDirection_cameraspace);
			// Direction in which the triangle reflects the light
			vec3 R = reflect(-l,n);
			// Cosine of the angle between the Eye vector and the Reflect vector,
			// clamped to 0
			//  - Looking into the reflection -> 1
			//  - Looking elsewhere -> < 1
			float cosAlpha = clamp( dot( E,R ), 0,1 );*/

			vec3 l = normalize( cameraPosition.xyz - lightPosition.xyz);
			vec3 R = reflect(-l, n);

			float cosAlpha = clamp(dot(vec3(0,0,-1), R), 0, 1);

			specular = pow(cosAlpha, 5);

			if(lightCastShadow)
			{
				vec4 shadowPos = biasMatrix * shadowMatrix * vec4(position,1.0);
				if(insideBox(shadowPos.xy, vec2(-1,-1), vec2(1,1)) > 0.1)
				{
/*						// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendiular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );
*/
					


					vec3 L = normalize( position.xyz - lightPosition.xyz);
					float cosTheta = clamp(dot(n,L), 0.0, 1.0);
					float bias = clamp(0.005*tan(acos(cosTheta)), 0.0001, 0.005);
					float bias = 0.0001;

					for (int i=0;i<4;i++){
						int index = i;
						visibility -= 0.15*(1.0-texture( s_shadowmap, vec3(shadowPos.xy + poissonDisk[index]/4000.0,  (shadowPos.z-bias)/shadowPos.w) ));
					}
				}
				else
					visibility = 1;
			}
			break;	
		case 1: // point light
			ambient = 0;
			vec3 lightDir = lightPosition.xyz - position.xyz;
			float distance = length(lightDir);

			if(lightCastShadow)
			{
				float sampledDistance = texture(s_shadowmapcube, normalize(lightDir * -1)).r;
				if(distance > sampledDistance+0.005)
					visibility = 0.4;
			}

			float distanceFac = pow(clamp((lightRange - distance) / lightRange, 0, 1), 1.5);
			diffuse = distanceFac * clamp(dot(normalize(normal), normalize(lightDir)), 0, 1);
			break;	
		case 2: // spotlight
		
			break;	
		default:
			fragColor = vec4(0,1,1,1);
			break;
	}



	fragColor = lightColor * (diffuse + ambient + specular) * visibility * image;
	fragColor.a = 1;
	
//	fragColor = lightColor * 0.1;
//	fragColor = vec4(0.15,0,0, 0.25);

	//fragColor.rgb = (normal.rgb);

	//fragColor.rgb = position.xyz;

}
