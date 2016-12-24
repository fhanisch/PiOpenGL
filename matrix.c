#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "matrix.h"

float *vecnf(float start, float step, uint count)
{
	uint i;
	float *v = malloc(count*sizeof(float));

	for (i=0;i<count;i++) v[i] = start + i*step;

	return v;
}

uint *vecni(uint start, uint count)
{
	uint i;
	uint *v = malloc(count*sizeof(uint));

	for (i=0;i<count;i++) v[i] = start + i;

	return v;
}

ushort *vecns(uint start, uint count)
{
	uint i;
	ushort *v = malloc(count*sizeof(ushort));

	for (i=0;i<count;i++) v[i] = start + i;

	return v;
}

Vector3 vec3(float x, float y, float z)
{
	Vector3 v = {x,y,z};
	return v;
}

pVector3 pTmpVec3(float x, float y, float z)
{
	static Vector3 v;
	v.x=x; v.y=y; v.z=z;
	return &v;
}

Vector4 getColor(float r, float g, float b, float a)
{
	Vector4 color={r, g, b, a};
	return color;
}

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

pMatrix4 pIdentity()
{
	pMatrix4 I = malloc(4*4*sizeof(float));
	memset(I,0,sizeof(Matrix4));
	I->m11 = 1.0f;
	I->m22 = 1.0f;
	I->m33 = 1.0f;
	I->m44 = 1.0f;
	return I;
}

Matrix4 scaleMatrix(Matrix4 M, Vector3 s)
{
	M.m11*=s.x;
	M.m22*=s.y;
	M.m33*=s.z;
	return M;
}

Matrix4 translateMatrix(Matrix4 M, Vector3 t)
{
	M.m14+=t.x;
	M.m24+=t.y;
	M.m34+=t.z;
	return M;
}

void translatePtrMatrix(pMatrix4 M, pVector3 t)
{
	M->m14+=t->x;
	M->m24+=t->y;
	M->m34+=t->z;
}

Matrix4 getRotZ(float phi)
{
	Matrix4 Z=identity();
	Z.m11 = cos(phi);	Z.m12 = sin(phi);
	Z.m21 = -sin(phi);	Z.m22 = cos(phi);
	return Z;
}

pMatrix4 pTmpGetRotZ(float phi)
{
	static Matrix4 Z;
	Z = identity();
	Z.m11 = cos(phi);	Z.m12 = sin(phi);
	Z.m21 = -sin(phi);	Z.m22 = cos(phi);
	return &Z;
}

Matrix4 transpose(Matrix4 M)
{
	unsigned int i,j;
	Matrix4 T;
	float *t=(float*)&T;
	float *m=(float*)&M;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			t[i+4*j] = m[j+4*i];
		}
	}
	return T;
}

pMatrix4 pTmpTranspose(pMatrix4 M)
{
	unsigned int i,j;
	static Matrix4 T;
	float *t=(float*)&T;
	float *m=(float*)M;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			t[i+4*j] = m[j+4*i];
		}
	}
	return &T;
}

Matrix4 multMatrix(Matrix4 L, Matrix4 R)
{
	Matrix4 M;
	float *m = (float*)&M;
	float *r = (float*)&R;
	float *l = (float*)&L;
	unsigned int i,j;

	for (j=0;j<4;j++)
		for (i=0;i<4;i++)
			m[4*j+i] = l[4*j]*r[i] + l[4*j+1]*r[i+4] + l[4*j+2]*r[i+8] + l[4*j+3]*r[i+12];

	return M;
}

Matrix4 multPtrMatrix(pMatrix4 L, pMatrix4 R)
{
	Matrix4 M;
	float *m = (float*)&M;
	float *r = (float*)R;
	float *l = (float*)L;
	unsigned int i,j;

	for (j=0;j<4;j++)
		for (i=0;i<4;i++)
			m[4*j+i] = l[4*j]*r[i] + l[4*j+1]*r[i+4] + l[4*j+2]*r[i+8] + l[4*j+3]*r[i+12];

	return M;
}
