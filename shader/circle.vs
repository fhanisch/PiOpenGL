//vertex shader

uniform mat4 mProj;
uniform mat4 mModel;
attribute float u;

void main()
{
    float x,y;

	x = cos(u*2.0*3.14159);
	y = -sin(u*2.0*3.14159);

	gl_Position = mProj * mModel * vec4(x, y, 0.0, 1.0);
}
