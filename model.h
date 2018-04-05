#pragma once
#pragma once
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<algorithm>
#include<list>
#include<GL/glut.h>
#include<math.h>
#include "matrix.h"
#include "vec4.h"
using namespace std;


class ModelFace {
public:
	std::vector<int> indices;
};

typedef struct {
	float r;
	float g;
	float b;
	float z;
}pixelData;

typedef struct {
	float xmin;
	float ymax;
	float dx;
	float dz;
	float ymin;
	float zfar;
	float znear;
	float r1;
	float g1;
	float b1;
	float r2;
	float g2;
	float b2;
} edge;

void printEdge(edge e) {
	printf("maxy is: %f, miny is: %f, minx is: %f, dx is: %f.\n",e.ymax,e.ymin,e.xmin,e.dx);
}

vec3 transfer4to3(vec4 vec) {
	vec3 temp;
	temp.x = vec.A[0] / vec.A[3];
	temp.y = vec.A[1] / vec.A[3];
	temp.z = vec.A[2] / vec.A[3];
	return temp;
}

vec4 transfer3to4(vec3 vec) {
	vec4 temp;
	temp.A[0] = vec.x;
	temp.A[1] = vec.y;
	temp.A[2] = vec.z;
	temp.A[3] = 1;
	return temp;
}

bool operator < (edge const & e1, edge const & e2) {
	if (e1.xmin < e2.xmin) {
		return true;
	}
	else {
		return false;
	}
}

class Model {

public:
	vector<ModelFace> faces;
	vector<vec3> vertices;
	int vertNum, faceNum;
	vector<vector<pixelData>> z_buffer;
	

public:	
	/*Load data from .txt file*/
	void loadModel(std::string path) {
		ifstream inFile;
		inFile.open(path);
		std::string line;
		inFile >> line;		
		inFile >> vertNum >> faceNum;

		faces.resize(faceNum);
		vertices.resize(vertNum);
		for (int i = 0; i < vertNum; i++) {
			float x, y, z;
			inFile >> x >> y >> z;
			vertices[i].x = x;
			vertices[i].y = y;
			vertices[i].z = z;
		}
		for (int i = 0; i < faceNum; i++) {
			int number;
			inFile >> number;
			faces[i].indices.resize(number);
			for (int j = 0; j < number; j++) {
				inFile >> faces[i].indices[j];
			}
		}

	}
	/*Back face culling function*/
	void backFaceCulling(vec4 direction) {

		vector<ModelFace> tempFaces;
		for (ModelFace face : faces) {
			int index1 = face.indices[0];
			int index2 = face.indices[1];
			int index3 = face.indices[2];
			vec4 vector1, vector2;
			vector1.A[0] = vertices[index2 - 1].x - vertices[index1 - 1].x;
			vector1.A[1] = vertices[index2 - 1].y - vertices[index1 - 1].y;
			vector1.A[2] = vertices[index2 - 1].z - vertices[index1 - 1].z;
			vector1.A[3] = 1;
			vector2.A[0] = vertices[index3 - 1].x - vertices[index2 - 1].x;
			vector2.A[1] = vertices[index3 - 1].y - vertices[index2 - 1].y;
			vector2.A[2] = vertices[index3 - 1].z - vertices[index2 - 1].z;
			vector2.A[3] = 1;
			vec4 norm = cross_product(vector1, vector2);
			float clippingNum = dot_product(direction, norm);
			if (clippingNum < 0) {
				faceNum--;
				continue;
			}
			else {
				tempFaces.push_back(face);
			}
		}
		faces = tempFaces;
	}
	/*render a model come with data after use transformations*/
	void renderModel(matrix base) {
		for (int i = 0; i < vertNum; i++) {
			vec4 vec4temp;
			vec4temp.A[0] = vertices[i].x;
			vec4temp.A[1] = vertices[i].y;
			vec4temp.A[2] = vertices[i].z;
			vec4temp.A[3] = 1;
			vec4temp = multi_vector(base, vec4temp);
			vec3 vec3temp;
			vec3temp = transfer4to3(vec4temp);
			vertices[i] = vec3temp;
			vertices[i].x = vertices[i].x / vertices[i].z;
			vertices[i].y = vertices[i].y / vertices[i].z;

		}
		for (int i = 0; i < faceNum; i++) {
			int num = faces[i].indices.size();
			glBegin(GL_LINE_LOOP);
			for (int j = 0; j < num; j++) {
				glVertex2f(vertices[faces[i].indices[j] - 1].x, vertices[faces[i].indices[j] - 1].y);
			}
			glEnd();
		}
	}
	/*--------------------This function is to start scanline algorithm---------------*/
	void myScanline(int width, int height) {
		z_buffer.resize(width);
		printf("size of window is %d * %d\n",width, height);
		for (int i = 0; i < width; i++) {
			z_buffer[i].resize(height);
		}
		vector<vec3> scaleVertices;
		scaleVertices.resize(vertNum);
		for (int i = 0; i < vertNum; i++) {
			scaleVertices[i].x = floor((vertices[i].x + 1)*(height / 2));
			scaleVertices[i].y = floor((vertices[i].y + 1)*(width / 2));
		}
		for (ModelFace face : faces) {
			fillPolygon(face, scaleVertices,width,height);
		}
	}

	void fillPolygon(ModelFace face, vector<vec3> v,int width, int height) {
		float red, green, blue;
		red = ((float)rand() / RAND_MAX);
		green = ((float)rand() / RAND_MAX);
		blue = ((float)rand() / RAND_MAX);
		int faceSize = face.indices.size();
		vector<int> faceIndices;
		faceIndices.resize(faceSize);
		for (int i = 0; i < faceSize; i++) {
			faceIndices[i] = face.indices[i] - 1;
		}
		/*--------Create ET------------*/
		vector<edge> edges;
		
		for (int i = 0; i < faceSize-1; i++) {
			edge tempEdge;
			if (v[faceIndices[i]].y == v[faceIndices[i + 1]].y) continue;
			else {
				tempEdge.ymax = max(v[faceIndices[i]].y, v[faceIndices[i + 1]].y);
				tempEdge.ymin = min(v[faceIndices[i]].y, v[faceIndices[i + 1]].y);
				tempEdge.xmin = (v[faceIndices[i]].y == tempEdge.ymax) ? v[faceIndices[i + 1]].x : v[faceIndices[i]].x;
				tempEdge.dx =  (v[faceIndices[i]].x - v[faceIndices[i + 1]].x) / (v[faceIndices[i]].y - v[faceIndices[i + 1]].y);
				tempEdge.zfar = max(v[faceIndices[i]].z, v[faceIndices[i + 1]].z);
				tempEdge.znear = min(v[faceIndices[i]].z, v[faceIndices[i + 1]].z);
				tempEdge.dz = (v[faceIndices[i]].z - v[faceIndices[i + 1]].z) / (v[faceIndices[i]].x - v[faceIndices[i + 1]].x);
			}
			edges.push_back(tempEdge);
		}
		if (v[faceIndices[faceSize - 1]].y != v[faceIndices[0]].y) {
			edge tempEdge;
			tempEdge.ymax = max(v[faceIndices[faceSize - 1]].y, v[faceIndices[0]].y);
			tempEdge.ymin = min(v[faceIndices[faceSize - 1]].y, v[faceIndices[0]].y);
			tempEdge.xmin = (v[faceIndices[faceSize - 1]].y == tempEdge.ymax) ? v[faceIndices[0]].x : v[faceIndices[faceSize - 1]].x;
			tempEdge.dx = (v[faceIndices[faceSize - 1]].x - v[faceIndices[0]].x) / (v[faceIndices[faceSize - 1]].y - v[faceIndices[0]].y);
			tempEdge.zfar = max(v[faceIndices[faceSize - 1]].z, v[faceIndices[0]].z);
			tempEdge.znear = min(v[faceIndices[faceSize - 1]].z, v[faceIndices[0]].z);
			tempEdge.dz = (v[faceIndices[faceSize - 1]].z - v[faceIndices[0]].z) / (v[faceIndices[faceSize - 1]].x - v[faceIndices[0]].x);
			edges.push_back(tempEdge);
		}

		int realSize = edges.size();
		int maxY = 0;
		int minY = 100000;
		for (edge e : edges) {
			if (e.ymax > maxY) maxY = (int)e.ymax;
			if (e.ymin < minY) minY = (int)e.ymin;
		}
		/*we need to sort edges with xmin smallest left*/

		std::sort(edges.begin(), edges.end());

		/*--------start scan------------------*/
		for (int yp = minY; yp < maxY; yp++) {
			for (int xp = 0; xp < height; xp++) {
				int count = 0;
				for (edge e : edges) {
					if (yp <= e.ymax&yp >e.ymin) {
						if (xp >= (e.xmin + e.dx*(yp - e.ymin))) {
							count++;
							//printf("position at %d,%d, is %d\n", xp, yp, count);
						}
					}
				}
				if (count % 2 == 1) {
					pixelData pd;
					pd.r = red;
					pd.g = green;
					pd.b = blue;
					
					float tempZ = 1;
					float realZ = 0;
					float ratio = (maxY - yp) / (maxY - minY);
					edge e = edges[count - 1];
					realZ = e.znear + ratio * (e.zfar - e.znear);
					pd.z = (realZ < tempZ) ? realZ : tempZ;
					z_buffer[yp][xp] = pd;
					/*z_buffer[width - 1 - xp][height - 1 - yp].r = R;
					z_buffer[width - 1 - xp][height - 1 - yp].g = G;
					z_buffer[width - 1 - xp][height - 1 - yp].b = B;
					float tempZ = 1;
					float realZ = 0;
					float ratio = (maxY - yp) / (maxY - minY);
					edge e = edges[count - 1];
					realZ = e.znear + ratio * (e.zfar - e.znear);
					z_buffer[width - 1 - xp][height - 1 - yp].z = (realZ > tempZ) ? tempZ : realZ;*/
				}
			}
		}		

	}
};
