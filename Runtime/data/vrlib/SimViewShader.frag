#version 150

in vec3 normal;

out vec4 fragColor;

void main()
{
	float diffuse = dot(normalize(vec3(1,1,1)), normalize(normal));
	diffuse = clamp(abs(diffuse), 0, 1);

	float light = 0.5 * diffuse + 0.5;

	fragColor = vec4(light, light, light, 1);
}
