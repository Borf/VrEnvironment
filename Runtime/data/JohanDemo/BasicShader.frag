uniform sampler2D s_texture;
varying vec2 texCoord;
varying vec3 normal;

void main()
{
	gl_FragColor = texture2D(s_texture, texCoord);
}
