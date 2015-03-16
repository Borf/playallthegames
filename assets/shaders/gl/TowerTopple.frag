uniform sampler2D s_texture;

varying vec3 normal;
varying vec2 texCoord;


void main()
{
	float c = 0.75 + clamp(0, 0.25, 0.25 * dot(normalize(vec3(1,1,1)), normalize(normal)));


	gl_FragColor = texture2D(s_texture, texCoord) * vec4(c,c,c,1);
}
