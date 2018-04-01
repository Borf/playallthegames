attribute vec2 a_position;
attribute vec2 a_texture;
varying vec2 texCoord;

uniform float transition;


void main()
{
	texCoord = a_texture;
	gl_Position = vec4(a_position + 
		vec2((1.0-transition) * 2.0 * sin((1.0-transition) * 4.0), 
			 (1.0-transition) * 2.0 * sin((1.0-transition) * 3.0)),0.0,1.0);
}
