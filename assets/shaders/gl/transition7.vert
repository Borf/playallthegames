attribute vec2 a_position;
attribute vec2 a_texture;
varying vec2 texCoord;

uniform float transition;


void main()
{
	texCoord = a_texture;
	gl_Position = vec4(a_position + 
		vec2((1-transition) * 2 * sin((1-transition) * 4), (1-transition) * 2 * sin((1-transition) * 3.0)),0.0,1.0);
}
