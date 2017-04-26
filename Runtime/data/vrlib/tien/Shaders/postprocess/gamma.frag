#version 150

uniform sampler2D s_color;
uniform sampler2D s_normal;
uniform sampler2D s_depth;

uniform float gamma = 1.0;

in vec2 texCoord;
out vec4 fragColor;



float toGamma(float v) {
  return pow(v, 1.0 / gamma);
}

vec2 toGamma(vec2 v) {
  return pow(v, vec2(1.0 / gamma));
}

vec3 toGamma(vec3 v) {
  return pow(v, vec3(1.0 / gamma));
}

vec4 toGamma(vec4 v) {
  return vec4(toGamma(v.rgb), v.a);
}



void main()
{
	fragColor = toGamma(texture2D( s_color, texCoord));
}
