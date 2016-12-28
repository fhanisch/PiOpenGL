//vertex shader

uniform mat4 mProj;
uniform mat4 mView;
uniform mat4 mModel;
uniform mat4 mTransInvModelView;

attribute float u;
attribute float v;

varying vec4 vertexPosition;
varying vec4 normalPosition;

const float pi = 3.14159;

void main()
{
    vec3 vertex;
	vec3 normal;

    float R=1.0;
    float u = 2.0*pi*u;
	float v = pi*v;

	vertex.x = R*sin(v)*cos(u);
	vertex.y = R*sin(v)*sin(u);
	vertex.z = R*cos(v);

	normal.x = sin(v)*cos(u);
	normal.y = sin(v)*sin(u);
	normal.z = cos(v);

	vertexPosition = mView * mModel * vec4(vertex, 1.0);
	//normalPosition = transpose(inverse(mView*mModel)) * vec4(normal, 1.0);
	normalPosition = mTransInvModelView * vec4(normal, 1.0);

	gl_Position = mProj * vertexPosition;
}
