#version 330

in vec3 position;
in vec3 worldNormal;
in vec3 eyeNormal;
out vec4 fragColor;

uniform vec3 eyePos;
uniform samplerCube envMap;

void main() {
     vec3 eye = normalize(eyePos - position);
     vec3 r = reflect(eye, worldNormal);
     vec4 color = texture(envMap, r);
     color.a = 0.5;
     fragColor = color;
}
