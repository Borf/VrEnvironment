#version 330

in vec3 position;
out vec4 fragColor;

uniform sampler2D glow;
uniform sampler2D color;
uniform vec3 sunDirection;

void main()
{
	vec3 pos = position;
	pos.y = max(pos.y, 0);

	vec3 V = normalize(pos);
	vec3 L = normalize(sunDirection);

	// Compute the proximity of this fragment to the sun.
	float vl = dot(V, L);
	// Look up the sky color and glow colors.
	vec4 Kc = texture2D(color, vec2((L.y + 1.0) / 2.0, 1-V.y));
	vec4 Kg = texture2D(glow,  vec2((L.y + 1.0) / 2.0, 1-vl));
	// Combine the color and glow giving the pixel value.
	fragColor = vec4(Kc.rgb + Kg.rgb * Kg.a / 2.0, Kc.a);
}
