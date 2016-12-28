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

void createMeshGrid(float **u, float **v, uint *uSize, uint *vSize, uint m, uint n)
{
	unsigned int i,j;

	*uSize = m*n*sizeof(float);
	*vSize = m*n*sizeof(float);
	*u = malloc(*uSize);
	*v = malloc(*vSize);

	for(i=0;i<m;i++)
		for(j=0;j<n;j++)
		{
			(*u)[i*n+j]=(float)j/((float)n-1);
			(*v)[j*m+i]=(float)j/((float)m-1);
		}
}

void createMeshGridIndices(ushort **indices, uint *indicesLen, uint *indicesSize, uint m, uint n)
{
	uint i,j;

	*indicesLen=3*2*(m-1)*(n-1);
	*indicesSize = *indicesLen * sizeof(ushort);
	*indices = malloc(*indicesSize);

	for(i=0;i<m-1;i++)
		for(j=0;j<n-1;j++)
		{
			(*indices)[6*(i*(n-1)+j)+0]=i*n+j;
			(*indices)[6*(i*(n-1)+j)+1]=i*n+j+1;
			(*indices)[6*(i*(n-1)+j)+2]=(i+1)*n+j;

			(*indices)[6*(i*(n-1)+j)+3]=i*n+j+1;
			(*indices)[6*(i*(n-1)+j)+4]=(i+1)*n+j;
			(*indices)[6*(i*(n-1)+j)+5]=(i+1)*n+j+1;
		}
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
	pMatrix4 I = malloc(sizeof(Matrix4));
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

void setPtrPosition(pMatrix4 M, pVector3 t)
{
	M->m14=t->x;
	M->m24=t->y;
	M->m34=t->z;
}

Matrix4 getRotY(float phi)
{
	Matrix4 Y=identity();
	Y.m11 = cos(phi);	Y.m13 = sin(phi);
	Y.m31 = -sin(phi);	Y.m33 = cos(phi);
	return Y;
}

pMatrix4 pTmpGetRotY(float phi)
{
	static Matrix4 Y;
	Y=identity();
	Y.m11 = cos(phi);	Y.m13 = sin(phi);
	Y.m31 = -sin(phi);	Y.m33 = cos(phi);
	return &Y;
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

Matrix4 getFrustum(float r, float t, float n, float f)
{
	Matrix4 F;

	F.m11=n/r;	F.m12=0;	F.m13=0;			F.m14=0;
	F.m21=0;	F.m22=n/t;	F.m23=0;			F.m24=0;
	F.m31=0;	F.m32=0;	F.m33=-(f+n)/(f-n);	F.m34=-2*f*n/(f-n);
	F.m41=0;	F.m42=0;	F.m43=-1;			F.m44=0;

	return F;
}
