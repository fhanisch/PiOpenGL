//vertex shader

uniform mat4 mProj;
uniform mat4 mView;
uniform mat4 mModel;
attribute vec3 vertex;

void main()
{
	gl_Position = mProj * mView * mModel * vec4(vertex,1.0);
}
