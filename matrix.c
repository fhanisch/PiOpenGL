#include <string.h>
#include "matrix.h"

Matrix4 identity()
{
	Matrix4 I;
	memset(&I,0,sizeof(Matrix4));
	I.m11 = 1.0f;
	I.m22 = 1.0f;
	I.m33 = 1.0f;
	I.m44 = 1.0f;

	return I;
}

Vector4 getColor(float r, float g, float b, float a)
{
	Vector4 color={r, g, b, a};

	return color;
}
