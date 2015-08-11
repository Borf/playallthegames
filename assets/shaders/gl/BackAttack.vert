#version 150
attribute vec3 a_position;
attribute vec2 a_texcoord;
attribute vec3 a_normal;

varying vec3 normal;
varying vec2 texCoord;


uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;

void main()
{
	texCoord = a_texcoord;

	mat3 normalMatrix = mat3(modelMatrix);
	normalMatrix = transpose(inverse(normalMatrix));
	normal = normalMatrix * a_normal;

	gl_Position = projectionMatrix * cameraMatrix * modelMatrix * vec4(a_position,1.0);
}