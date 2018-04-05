#include <stdlib.h>
#include<string>
#include <math.h>
#include<stdio.h>
#include<GL\glut.h>
#include<vector>
#include <list>
#include "matrix.h"
#include "vec4.h"
#include "model.h"

#define PI 3.1415926
using namespace::std;
vec4 viewDirection;
matrix baseMatrix;
const int screenX = 450;
const int screenY = 70;
const int screenHeight = 640;
const int screenWidth = 640;
vector<vector<pixelData>> z_buffer(screenHeight, vector<pixelData>(screenWidth));
float frameBuffer[screenHeight][screenWidth][3];
vector<Model> modelArray;

/*-----my gl function definition---------------*/
void myLoadIdentity();
void myLookAt(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz);
void myPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);
void init_z_buffer();
void init_color_buffer();
void combineZbuffer(vector<Model> modelArray);


//Render function
void renderScene(void) {
	init_z_buffer();
	myLoadIdentity();	
	myPerspective(60.0f, 1.0f, 0.1f, 100.0f);
	myLookAt(10.0f, 10.0f, 10.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);
	Model model1;
	Model model2;
	model1.loadModel("knight.d.txt");
	model2.loadModel("queen.d.txt");
	model1.backFaceCulling(viewDirection);
	model2.backFaceCulling(viewDirection);
	model1.renderModel(baseMatrix);
	model2.renderModel(baseMatrix);
	model1.myScanline(screenWidth, screenHeight);
	model2.myScanline(screenWidth, screenHeight);
	modelArray.push_back(model1);
	modelArray.push_back(model2);
	combineZbuffer(modelArray);
	init_color_buffer();
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, frameBuffer);
	glutSwapBuffers();
	glFlush();
}


int main(int argc, char **argv) {
	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(screenX, screenY);
	glutInitWindowSize(screenHeight,screenWidth);
	glutCreateWindow("ComputerGraphics");
	glutDisplayFunc(renderScene);
	glutMainLoop();
	return 1;
}


/*Function details*/
//load identity matrix for transform matrix.
void myLoadIdentity(void) {
	baseMatrix = identityMatrix();
}
//implemention of gluLookAt()
void myLookAt(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz) {
	vec4 forward, side, direction, up;
	matrix m;
	forward.A[0] = centerx - eyex;
	forward.A[1] = centery - eyey;
	forward.A[2] = centerz - eyez;
	forward.A[3] = 1.0;

	forward = normalize(forward);
	viewDirection = forward;
	direction.A[0] = upx;
	direction.A[1] = upy;
	direction.A[2] = upz;
	direction.A[3] = 1.0;

	side = cross_product(forward, direction);
	side = normalize(side);
	up = cross_product(side, forward);

	m.A[0][0] = side.A[0];
	m.A[0][1] = side.A[1];
	m.A[0][2] = side.A[2];
	m.A[0][3] = 0.0;

	m.A[1][0] = up.A[0];
	m.A[1][1] = up.A[1];
	m.A[1][2] = up.A[2];
	m.A[1][3] = 0.0;

	m.A[2][0] = forward.A[0];
	m.A[2][1] = forward.A[1];
	m.A[2][2] = forward.A[2];
	m.A[2][3] = 0.0;

	m.A[3][0] = 0.0;
	m.A[3][1] = 0.0;
	m.A[3][2] = 0.0;
	m.A[3][3] = 1.0;

	matrix T = return_translate(-eyex, -eyey, -eyez);
	baseMatrix = multi_matrix(baseMatrix, m);
	baseMatrix = multi_matrix(baseMatrix, T);
}
//implemention of gluPerspective();
void myPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
	float theta = fovy * PI / 360.0;
	float h = zNear * tan(theta);
	float xmax = h * aspect;
	matrix Pers;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) Pers.A[i][j] = 0;
	}

	Pers.A[0][0] = zNear / h;
	Pers.A[1][1] = zNear / h;
	Pers.A[2][2] = (zFar + zNear) / (zFar - zNear);
	Pers.A[3][2] = (2 * zFar*zNear) / (zFar - zNear);
	Pers.A[2][3] = -1;
	Pers.A[3][3] = 0;
	baseMatrix = multi_matrix(baseMatrix, Pers);
}

void init_z_buffer()
{
	for (int i = 0; i < screenHeight; i++) {
		for (int j = 0; j < screenWidth; j++) {
			z_buffer[i][j].z = 1;
		}
	}
}

void init_color_buffer() {
	for (int i = 0; i < screenHeight; i++) {
		for (int j = 0; j < screenWidth; j++) {
			frameBuffer[i][j][0] = z_buffer[i][j].r;
			frameBuffer[i][j][1] = z_buffer[i][j].g;
			frameBuffer[i][j][2] = z_buffer[i][j].b;
		}
	}
}

void combineZbuffer(vector<Model> modelArray) {
	int size = modelArray.size();
	z_buffer = modelArray[0].z_buffer;
	for (int i = 1; i < size; i++) {
		int height = modelArray[i].z_buffer.size();
		int width = modelArray[i].z_buffer[0].size();
		for (int xIndex = 0; xIndex < height; xIndex++) {
			for (int yIndex = 0; yIndex < width; yIndex++) {
				if (z_buffer[xIndex][yIndex].z > modelArray[i].z_buffer[xIndex][yIndex].z) {
					z_buffer[xIndex][yIndex].z = modelArray[i].z_buffer[xIndex][yIndex].z;
					z_buffer[xIndex][yIndex].r = modelArray[i].z_buffer[xIndex][yIndex].r;
					z_buffer[xIndex][yIndex].g = modelArray[i].z_buffer[xIndex][yIndex].g;
					z_buffer[xIndex][yIndex].b = modelArray[i].z_buffer[xIndex][yIndex].b;
				}
			}
		}
	}
}

