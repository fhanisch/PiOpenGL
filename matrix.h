#ifndef MATRIX_H
#define MATRIX_H

typedef struct
{
	float x, y, z;
} Vector3;

typedef struct
{
	float x, y, z, w;
} Vector4;

typedef struct
{
	float m11, m12, m13, m14;
	float m21, m22, m23, m24;
	float m31, m32, m33, m34;
	float m41, m42, m43, m44;

} Matrix4;

Vector3 vec3(float x, float y, float z);
Vector4 getColor(float r, float g, float b, float a);
Matrix4 identity();
Matrix4 scaleMatrix(Matrix4 M, Vector3 s);
Matrix4 translateMatrix(Matrix4 M, Vector3 t);
Matrix4 transpose(Matrix4 M);

#endif
