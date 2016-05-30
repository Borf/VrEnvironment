#if 0

vec3 encodeNormal(vec3 n) {
	return n * 0.5 + 0.5;
}

vec3 decodeNormal(vec3 renc) {
	return renc*2.0-1.0;
}
#endif



#if 1
//Cry Engine 3
//http://aras-p.info/texts/CompactNormalStorage.html
vec2 encodeNormal(vec3 n) {
	float p = sqrt(n.z*8+8);
	return vec2(n.xy/p + 0.5);
}

vec3 decodeNormal(vec2 enc) {
	vec2 fenc = enc*4.0-2.0;
	float f = dot(fenc,fenc);
	float g = sqrt(1-f/4.0);
	vec3 n;
	n.xy = fenc*g;
	n.z = 1-f/2;
	return n;
}

#endif



#if 0

vec2 encodeNormal(vec3 n) {
	return n.xy * inversesqrt(8 - 8 * n.z) + 0.5;
}

vec3 decodeNormal(vec2 renc) {
	vec2 enc = 2 * renc - 1;
	float len2 = dot(enc, enc);
	return vec3(enc * sqrt(4 - 4 * len2), 2 * len2 - 1);
}


#endif


#if 0

const float EPSILON = 0.001;

vec2 encodeNormal (vec3 normal)
{
	// Project normal positive hemisphere to unit circle
	// We project from point (0,0,-1) to the plane [0,(0,0,-1)]
	// den = dot (l.d, p.n)
	// t = -(dot (p.n, l.p) + p.d) / den
	vec2 p = normal.xy / (abs (normal.z) + 1.0);

	// Convert unit circle to square
	// We add epsilon to avoid division by zero
	float d = abs (p.x) + abs (p.y) + EPSILON;
	float r = length (p);
	vec2 q = p * r / d;

	// Mirror triangles to outer edge if z is negative
	float z_is_negative = max (-sign (normal.z), 0.0);
	vec2 q_sign = sign (q);
	q_sign = sign (q_sign + vec2 (0.5, 0.5));
	// Reflection
	// qr = q - 2 * n * (dot (q, n) - d) / dot (n, n)
	q -= z_is_negative * (dot (q, q_sign) - 1.0) * q_sign;

	return q;
}


vec3 decodeNormal (vec2 encodedNormal)
{
	vec2 p = encodedNormal;
	
	// Find z sign
	float zsign = sign (1.0 - abs (p.x) - abs (p.y));
	// Map outer triangles to center if encoded z is negative
	float z_is_negative = max (-zsign, 0.0);
	vec2 p_sign = sign (p);
	p_sign = sign (p_sign + vec2 (0.5, 0.5));
	// Reflection
	// qr = q - 2 * n * (dot (q, n) - d) / dot (n, n)
	p -= z_is_negative * (dot (p, p_sign) - 1.0) * p_sign;

	// Convert square to unit circle
	// We add epsilon to avoid division by zero
	float r = abs (p.x) + abs (p.y);
	float d = length (p) + EPSILON;
	vec2 q = p * r / d;

	// Deproject unit circle to sphere
	float den = 2.0 / (dot (q, q) + 1.0);
	vec3 v = vec3(den * q, zsign * (den - 1.0));

	return v;
}

#endif