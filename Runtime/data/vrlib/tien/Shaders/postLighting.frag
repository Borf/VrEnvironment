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
uniform float lightIntensity;
uniform float lightSpotAngle;
uniform float lightAmbient;
uniform float lightCutoff;
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

    vec4 image = texture2D( s_color, texCoord );									//albedo texture
    float depth = texture2D( s_depth, texCoord ).x;									//depth
	vec4 normalEncoded = texture2D( s_normal, texCoord );
	float shinyness = normalEncoded.a;												// shinyness is encoded in the alpha of the normal buffer
    vec3 normal = normalize(decodeNormal(normalEncoded.xyz));						//normal in world space

	vec4 viewPos = vec4(texCoord.xy*2.0-1.0, depth*2.0-1.0, 1);						//position relative to view
	vec4 tempPos = modelViewMatrixInv * projectionMatrixInv * viewPos;
	vec3 position = tempPos.xyz / tempPos.w;										//position in world coordinats
    
	float diffuse = 0;																//final diffuse factor
	float ambient = 0;																//final ambient factor
	float specular = 0;																//final specular factor
	float visibility = 1.0;															//shadow occlusion. 1 is visible, 0 is occluded by shadow
	vec3 lightDir = lightPosition.xyz - position.xyz;
	float distance = length(lightDir);

	switch(lightType)
	{
		case 0:																		// directional light
																					// for a directional light, the position is actually the direction...for now
			diffuse = max(0, dot(normal, normalize(lightDirection.xyz))) * 0.5;
			ambient = 0.5;


			//if(shinyness > 0)
			{
				vec3 l = normalize( lightDirection.xyz);
				vec3 R = normalize(reflect(-l, normal));
				float cosAlpha = clamp(dot(normalize(cameraPosition - position.xyz), R), 0, 1);
				specular = pow(cosAlpha, 10) * shinyness;
//				specular = clamp(specular, 0, 1);
			}

			if(lightCastShadow)
			{
				vec4 shadowPos = biasMatrix * shadowMatrix * vec4(position,1.0);
				if(insideBox(shadowPos.xy, vec2(-1,-1), vec2(1,1)) > 0.1)
				{
					float bias = 0.0001;
					for (int i=0;i<4;i++){
						visibility -= 0.15*(1.0-texture( s_shadowmap, vec3(shadowPos.xy + poissonDisk[i]/4000.0,  (shadowPos.z-bias)/shadowPos.w) ));
					}
				}
				else
					visibility = 1;
			}
			break;	
		case 1: // point light
			ambient = 0;

			if(lightCastShadow)
			{
				float sampledDistance = texture(s_shadowmapcube, normalize(lightDir * -1)).r;
				if(distance > sampledDistance+0.005)
					visibility = 0.4;
			}

			//float attenuation = 1.0 / (1.0 + (2.0 / lightRange) * distance + (1.0 / (lightRange * lightRange)) * distance * distance);
			
			float d = max(distance - lightRange, 0.0);
			float denom = d / lightRange + 1;
			float attenuation = lightIntensity / (denom * denom);
			if(lightCutoff > 0)
				attenuation = max(0, (attenuation - lightCutoff) / (1 - lightCutoff));

			attenuation = attenuation;

//			float distanceFac = pow(clamp((lightRange - distance) / lightRange, 0, 1), 1.5);
			diffuse = attenuation * clamp(dot(normalize(normal), normalize(lightDir)), 0, 1);
			break;	
		case 2: // spotlight
			ambient = 0;
			
			if(lightCastShadow)
			{
				vec4 shadowPos = biasMatrix * shadowMatrix * vec4(position,1.0);
//				if(insideBox(shadowPos.xy, vec2(-1,-1), vec2(1,1)) > 0.1)
				{
					float bias = 0.0001;
					for (int i=0;i<4;i++){
						visibility -= 0.15*(1.0-textureProj( s_shadowmap, vec4(shadowPos.xy + poissonDisk[i]/4000.0,  (shadowPos.z-bias), shadowPos.w) ));
					}
				}
//				else
//					visibility = 1;
			}
			float distanceFac2 = pow(clamp((lightRange - distance) / lightRange, 0, 1), 1.5);
			diffuse = distanceFac2 * clamp(dot(normalize(normal), normalize(lightDir)), 0, 1);

			float angle = acos(dot(normalize(lightDir), normalize(lightDirection)));
			if(abs(angle) > lightSpotAngle)
				diffuse = 0;
			else
				diffuse = diffuse * clamp(pow(abs(angle-lightSpotAngle), 0.45),0,1.5);

			break;	
		default:
			fragColor = vec4(0,1,1,1);
			break;
	}



	fragColor = lightColor * (diffuse + ambient + specular) * visibility * image;
	fragColor.a = 1;
}
