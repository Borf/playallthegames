#version 150
uniform sampler2D s_texture;
uniform vec4 color;

in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;


void main()
{
	const float fac = 0.5;

	float c = (1.0 - fac) + clamp(fac * abs(dot(normalize(vec3(1.0,1.0,-1.0)), normalize(normal))), 0.0, fac);


	fragColor = color * texture2D(s_texture, texCoord) * vec4(c,c,c,1.0);
}
