uniform sampler2D s_texture;
uniform vec4 color;

varying vec3 normal;
varying vec2 texCoord;

varying vec3 lightDir;

void main()
{
	float c = clamp(dot(normalize(lightDir), normalize(normal)), 0, 1);

	c = 0.5 + 0.5 * c;

	gl_FragColor = texture2D(s_texture, texCoord) * vec4(c,c,c,1) + color;
}
