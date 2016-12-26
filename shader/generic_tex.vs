//vertex shader

uniform mat4 mProj;
uniform mat4 mModel;
attribute vec3 vertex;
attribute vec2 texcoord;

varying vec2 texCoordOut;

void main()
{
	texCoordOut = texcoord;
	gl_Position = mProj * mModel * vec4(vertex,1.0);
}
