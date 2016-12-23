//vertex shader

uniform mat4 mProj;
uniform mat4 mModel;
attribute vec3 vertex;

void main()
{
	gl_Position = mProj * mModel * vec4(vertex,1.0);
}
