//vertex shader

uniform mat4 mProj;
attribute vec3 vertex;

void main()
{	
	gl_Position = mProj * vec4(vertex,1.0);
}
