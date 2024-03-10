#pragma once
#include <SDL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "Perlin.h"
//#include "ShaderManager.h"
#include <random>
#include <ctime>

using namespace std;


class Terrain {

public:
	//Constructors (default & constructor containing the position, width and height of the terrain)
	Terrain();
	Terrain(glm::vec3 position, int OffsetX, int OffsetY, float PerlinFrequency, float PerlinPersistence, float PerlinRandomNoise);
	//destructor
	~Terrain();
	//Methods
	void GenerateTerrain(float frequency, float persistence, float randomNoise);
	void DrawTerrain(GLuint uniformIndex, glm::mat4& modelview, glm::mat4& projection, GLuint shaderProgram, GLuint Texture);
	void CheckPlayerPosition(glm::vec3 playerPosition);
	void OutputPerlinValues();
	//Setters
	void SetPosition(glm::vec3 tPosition) { terrainPosition = tPosition; }
	//Getters
	glm::vec3 GetPosition() { return terrainPosition; };
	glm::vec2 GetSeamPosition() { return seamPosition; };

	bool toggleWireframe;	//Wireframe toggle
	GLdouble north, south, east, west;	//variables to hold the north, south, east and west of the terrain
	bool generateNorthTerrain, generateSouthTerrain, generateEastTerrain, generateWestTerrain;
	bool northHasBeenGenerated, southHasBeenGenerated, eastHasBeenGenerated, westHasBeenGenerated;
	glm::vec3 front_bottom_left, back_top_right;
	std::vector<glm::vec3> box_vertices;
	bool ActiveTerrain;
	vector<glm::vec3> gridNormals;

private:
	//Private methods
	void initializeGrid(int width, int height);
	void RecalculateHeightMesh(std::vector<float> newYVertices);
	void renderTerrain(GLenum drawMode, GLuint texture);
	void GenerateBoundingBox();
	void DrawBoundingBox();

	//Perlin/Simplex Noise 
	Perlin perlin;
	PerlinValues perlinValues;
	float randomFrequency, randomAmp, randomNoise, randomHeight;

	//Values to store the width and height of the terrain that is then filtered in to the perlin noise
	int terrainWidth, terrainHeight;
	int textureOffsetX, textureOffsetY;

	//Terrain transform values
	glm::vec3 terrainPosition;
	glm::vec2 seamPosition;
	GLfloat terrainScale, terrainRotX, terrainRotY, terrainRotZ, terrainAngle;

	//Variables for loading, storing and manipulating the terrain
	vector<glm::vec3> grid;
	vector<int> Indices;
	vector<unsigned int> indices;
	float* newVerts;
	std::vector<glm::vec2> terrainUV;
	GLuint terrainVertexBuffer, terrainNormalBuffer, terrainUVBuffer, terrainIndicesBuffer;
	glm::mat4 terrain_Modelview, terrain_Projection;

	GLenum drawMode;
	bool northOccupied, southOccupied, eastOccupied, westOccupied;
	GLuint boxBuffer;

	GLfloat detectionDistance;
};