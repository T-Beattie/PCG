#pragma once
//GLM Functions
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include <SDL.h>
#include <glew.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

//struct holding the Perlin Noise values to be manipulated
struct PerlinValues {
	float frequency;
	float persistence;
	float randomNoise;
};

class Perlin
{
public:
	static const GLuint MAXWIDTH = 100;
	static const GLuint MAXHEIGHT = 100;
	// Constructor
	Perlin();
	//function for generating the noise
	void GeneratePerlinNoise(float baseFreq, float persistence, int w, int h, float randomValue, int offsetX, int offsetY);
	GLubyte* getPerlinData();
	//Grabs the edge points of each terrain 
	void GrabEdgeValues();
	float turbulence(glm::vec3 point, float f);
	//containers for the generated perlin values
	vector<float> averageVector, newVec;
	vector<float> northEdge, southEdge, westEdge, eastEdge;

	GLuint texID;

private:
	GLubyte* data;
	vector<glm::vec2> Data;
	//Voronoi voronoi;

	int width;
	int height;
};

