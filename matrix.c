#include <string.h>
#include "matrix.h"

Vector3 vec3(float x, float y, float z)
{
    Vector3 v;
    v.x = x; v.y = y; v.z = z;
    return v;
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
