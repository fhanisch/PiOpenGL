#ifndef MATRIX_H
#define MATRIX_H

typedef struct
{
	float v1, v2, v3, v4;
} Vector4;

typedef struct
{
	float m11, m12, m13, m14;
	float m21, m22, m23, m24;
	float m31, m32, m33, m34;
	float m41, m42, m43, m44;

} Matrix4;

Matrix4 identity();
Vector4 getColor(float r, float g, float b, float a);

#endif
