#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 90) out; //max 256
out vec2 tex_coord;
uniform float time;   

//uniform float random value, we randomize this in the grass.cpp

uniform mat4 modelViewProjectionMatrix;
uniform float r1; //1.0f  //how to update in main shader function
uniform float r2; //0.0f	

vec4 explode(vec4 position, vec3 normal)
{
   // vec3 direction = normal; 
	//position.x = rand(position.xy);
	//position.y = rand(position.xy);
    return vec4(normal, 1.0f);
}

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position);
   return a;
}

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}



// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }




//randomize in geometry shader
//for loop met end primitive in loop met random grootte in shaders dus
//gl_Position[0].xyz gl_Position[1].xyz   gl_Position[2].xyz  enzovoort
//Convert the grass blade to 1 or 2 triangles instead of 3


float rand3d(vec3 a){
	return fract(a.z + sin(a.x*a.y*1424.0) * 12345.2);
} 
float rand;
float randTwo;
void main()
{
	//vec4 p = modelViewProjectionMatrix * gl_in[0].gl_Position;
	//if(p.z > 15)
	//	return;


	vec3 normal = vec3(0,0,0);

	const vec2 offset = vec2(0.5,0);	
	const vec2 fac = vec2(40,3);

	const float waveFactor = 0.1;
	
	vec3 a = vec3(gl_in[0].gl_Position.x, gl_in[0].gl_Position.y, gl_in[0].gl_Position.z);
	vec3 b = vec3(gl_in[1].gl_Position.x, gl_in[1].gl_Position.y, gl_in[1].gl_Position.z);
	vec3 c = vec3(gl_in[2].gl_Position.x, gl_in[2].gl_Position.y, gl_in[2].gl_Position.z);

	for(int i = 0; i < 30; i++)
	{
	float inputs = ((cos(i/0.8) + 8.0) / 3.0)  * i;
	float inputsTwo = ((sin(i/0.8) + 8.0) / 3.0) * i;

     rand   =	random(inputs);              // Random per-pixel value
	 randTwo = random(inputsTwo);  

	float normalX = (1 - sqrt(rand)) * gl_in[0].gl_Position.x + (sqrt(rand) * (1 -  randTwo)) * gl_in[1].gl_Position.x + (sqrt(rand) * randTwo) * gl_in[2].gl_Position.x;
	float normalY = (1 - sqrt(rand)) * gl_in[0].gl_Position.y + (sqrt(rand) * (1 - randTwo)) * gl_in[1].gl_Position.y + (sqrt(rand) * randTwo) * gl_in[2].gl_Position.y;
	float normalZ = (1 - sqrt(rand)) * gl_in[0].gl_Position.z + (sqrt(rand) * (1 - randTwo)) * gl_in[1].gl_Position.z + (sqrt(rand) * randTwo) * gl_in[2].gl_Position.z;
	normal = vec3(normalX, normalY, normalZ);

	//voor random pak je de i met r1 of r2

	float animationOffset = 1.4 * cos(time + gl_in[0].gl_Position.x + gl_in[0].gl_Position.z);

	
	gl_Position = modelViewProjectionMatrix * vec4(normal, 1.0f) + vec4(-0.0075 , 0.0, 0.0, 0.0); //+ rand(gl_in[i].gl_Position.xy);
	tex_coord = vec2(-0.01, 0)*fac+offset;
    EmitVertex();

    gl_Position = modelViewProjectionMatrix * vec4(normal, 1.0f) + vec4(0.075 , 0.0, 0.0, 0.0) ;
	tex_coord = vec2(0.01, 0)*fac+offset;
    EmitVertex();

    gl_Position = modelViewProjectionMatrix *vec4(normal, 1.0f)+ vec4(-0.075 + animationOffset * 2.0 * waveFactor, 2.0, 0.0, 0.0);
	tex_coord = vec2(-0.01, 0.3)*fac+offset;

    EmitVertex();
	EndPrimitive();

	}
}