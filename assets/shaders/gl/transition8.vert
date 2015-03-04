attribute vec2 a_position;
attribute vec2 a_texture;
varying vec2 texCoord;



void main()
{
	texCoord = a_texture;
	gl_Position = vec4(a_position,0.0,1.0);
}
