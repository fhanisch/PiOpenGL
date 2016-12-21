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

Vector4 getGreen()
{
	Vector4 green={0.0f, 1.0f, 0.0f, 0.5f};
	
	return green;
}
