#ifndef MATRIX_H
#define MATRIX_H

typedef struct
{
	float x, y, z;
} Vector3, *pVector3;

typedef struct
{
	float x, y, z, w;
} Vector4, *pVector4;

typedef struct
{
	float m11, m12, m13, m14;
	float m21, m22, m23, m24;
	float m31, m32, m33, m34;
	float m41, m42, m43, m44;

} Matrix4, *pMatrix4;

float *vecnf(float start, float step, uint count);
uint *vecni(uint start, uint count);
ushort *vecns(uint start, uint count);
void createMeshGrid(float **u, float **v, uint *uSize, uint *vSize, uint m, uint n);
void createMeshGridIndices(ushort **indices, uint *indicesLen, uint *indicesSize, uint m, uint n);
Vector3 vec3(float x, float y, float z);
pVector3 pTmpVec3(float x, float y, float z);
Vector4 getColor(float r, float g, float b, float a);
Matrix4 identity();
pMatrix4 pIdentity();
void setIdentity(pMatrix4 M);
Matrix4 scaleMatrix(Matrix4 M, Vector3 s);
Matrix4 translateMatrix(Matrix4 M, Vector3 t);
void translatePtrMatrix(pMatrix4 M, pVector3 t);
void setPtrPosition(pMatrix4 M, pVector3 t);
Matrix4 transpose(Matrix4 M);
Matrix4 getRotX(float phi);
pMatrix4 pTmpGetRotX(float phi);
Matrix4 getRotY(float phi);
pMatrix4 pTmpGetRotY(float phi);
Matrix4 getRotZ(float phi);
pMatrix4 pTmpGetRotZ(float phi);
pMatrix4 pTmpTranspose(pMatrix4 M);
Matrix4 multMatrix(Matrix4 L, Matrix4 R);
Matrix4 multPtrMatrix(pMatrix4 L, pMatrix4 R);
Matrix4 getFrustum(float r, float t, float n, float f);

#endif // MATRIX_H
