
#include <stdlib.h>
#include <math.h>
#include<stdio.h>
#include "vec4.h"
/*
define matrix.
provide matrix calculation functions.
*/
typedef struct {
	float A[4][4];
}matrix;

matrix multi_matrix(matrix m1, matrix m2)
{
	matrix M;
	int i, j, k;
	float sum;
	for (i = 0; i<4; i++)
	{
		for (j = 0; j<4; j++)
		{
			sum = 0.0;
			for (k = 0; k<4; k++)
			{
				sum = sum + m1.A[i][k] * m2.A[k][j];
			}
			M.A[i][j] = sum;
		}
	}
	return M;
}

matrix return_translate(float x, float y, float z)
{
	//Matrix *M=(Matrix*)malloc(sizeof(Matrix));
	matrix M;
	float array[4][4] = { { 1.0,0.0,0.0,x },{ 0.0,1.0,0.0,y },{ 0.0,0.0,1.0,z },{ 0.0,0.0,0.0,1.0 } };
	int i, j;
	for (i = 0; i<4; i++)
	{
		for (j = 0; j<4; j++)
		{
			M.A[i][j] = array[i][j];
		}
	}
	return M;
}

matrix identityMatrix() {
	matrix m;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == j) m.A[i][j] = 1;
			else m.A[i][j] = 0;
		}
	}
	return m;
}

vec4 multi_vector(matrix m, vec4 v)
{
	int i, j;
	float sum;
	vec4 V;
	for (i = 0; i < 4; i++)
	{
		sum = 0.0;
		for (j = 0; j<4; j++)
		{
			sum = sum + m.A[i][j] * v.A[j];
		}
		V.A[i] = sum;

	}
	return V;
}

void printMatrix(matrix m)
{
	int i, j;
	for (i = 0; i<4; i++)
	{
		for (j = 0; j<4; j++)
		{
			printf("%f  ", m.A[i][j]);
		}
		printf("\n");
	}
}
