uniform sampler2D s_texture;
uniform vec4 color;

varying vec3 normal;
varying vec2 texCoord;


void main()
{
	const float fac = 0.5;

	float c = (1 - fac) + clamp(fac * abs(dot(normalize(vec3(1,1,-1)), normalize(normal))), 0, fac);


	gl_FragColor = color * texture2D(s_texture, texCoord) * vec4(c,c,c,1);
}
