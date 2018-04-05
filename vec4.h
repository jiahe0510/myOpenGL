
#include<stdlib.h>
#include<math.h>
#include<stdio.h>

/*
Define vector
Provide vector calculate functions.
*/

typedef struct {
	float A[4];
}vec4;

typedef struct {
	float x, y, z;
}vec3;

vec4 cross_product(vec4 v1, vec4 v2)
{
	vec4 v;
	v.A[0] = v1.A[1] * v2.A[2] - v1.A[2] * v2.A[1];
	v.A[1] = v1.A[2] * v2.A[0] - v1.A[0] * v2.A[2];
	v.A[2] = v1.A[0] * v2.A[1] - v1.A[1] * v2.A[0];
	v.A[3] = 1.0;
	return v;

}

vec4 normalize(vec4 v1)
{
	float k = sqrt(v1.A[0] * v1.A[0] + v1.A[1] * v1.A[1] + v1.A[2] * v1.A[2]);
	vec4 v;
	v.A[0] = v1.A[0] / k;
	v.A[1] = v1.A[1] / k;
	v.A[2] = v1.A[2] / k;
	v.A[3] = 1.0;
	return v;
}

void print_vector4(vec4 v)
{
	int i;
	for (i = 0; i<4; i++)
	{
		printf("%f  ", v.A[i]);
	}
	printf("\n");
}

void print_vector3(vec3 v)
{
	printf("%f  ", v.x);
	printf("%f  ", v.y);
	printf("%f  ", v.z);
	printf("\n");
}

float dot_product(vec4 v1, vec4 v2) {
	float result;
	result = v1.A[0] * v2.A[0] + v1.A[1] * v2.A[1] + v1.A[2] * v2.A[2];
	return result;
}

vec4 subtraction(vec4 v1, vec4 v2)
{
	vec4 v;
	int i;
	for (i = 0; i<4; i++)
	{
		v.A[i] = v1.A[i] - v2.A[i];
	}
	v.A[3] = 1;
	return v;
}
