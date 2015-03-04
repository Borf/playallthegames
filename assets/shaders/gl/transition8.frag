uniform sampler2D s_texture;
varying vec2 texCoord;

uniform float transition;

void main()
{
	gl_FragColor = texture2D(s_texture, texCoord + (1 - transition) * vec2(cos(texCoord.x * 10 + texCoord.y*3), sin(texCoord.x * 3 + texCoord.y * 20)));
}