uniform sampler2D s_texture;
uniform sampler2D s_visionTexture;
uniform float zombieFactor;


varying vec2 texCoord;
varying vec4 color;


void main()
{
	vec4 vision = texture2D(s_visionTexture, gl_FragCoord.xy / vec2(1920,1080));

	if(zombieFactor > 0.1)
	{
		if(vision.r < 1.0)
			discard;
	}	

	gl_FragColor = color*texture2D(s_texture, texCoord)*vision;
}
