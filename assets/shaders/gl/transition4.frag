uniform sampler2D s_texture;
varying vec2 texCoord;

uniform float transition;

void main()
{
	gl_FragColor = texture2D(s_texture, texCoord);
}