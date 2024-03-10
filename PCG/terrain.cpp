#include "terrain.h"

Terrain::Terrain()
{
}

Terrain::Terrain(glm::vec3 position, int OffsetX, int OffsetY, float PerlinFrequency, float PerlinPersistence, float PerlinRandomNoise)
{
	terrainScale = 10.0f;
	terrainPosition = position;
	toggleWireframe = false;

	perlinValues.frequency = PerlinFrequency;
	perlinValues.persistence = PerlinPersistence;
	perlinValues.randomNoise = PerlinRandomNoise;

	terrainWidth = 100;
	terrainHeight = 100;

	initializeGrid(terrainWidth, terrainHeight);

	textureOffsetX = OffsetX;
	textureOffsetY = OffsetY;
	seamPosition.x = OffsetX;
	seamPosition.y = OffsetY;

	detectionDistance = 250.0f;
	northHasBeenGenerated = false;
	southHasBeenGenerated = false;
	eastHasBeenGenerated = false;
	westHasBeenGenerated = false;
	GenerateBoundingBox();
}

Terrain::~Terrain()
{
}

void Terrain::GenerateTerrain(float frequency, float persistence, float randomNoise)
{
	clock_t begin, end;
	double time_spent;
	begin = clock();
	perlin.GeneratePerlinNoise(frequency, persistence, terrainWidth, terrainHeight, randomNoise, textureOffsetX, textureOffsetY);
	//perlin.addVoronoi();

	RecalculateHeightMesh(perlin.averageVector);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	cout << "Time Spent on execution: " << time_spent << endl;
	OutputPerlinValues();
}

void Terrain::DrawTerrain(GLuint uniformIndex, glm::mat4& modelview, glm::mat4& projection, GLuint shaderProgram, GLuint Texture)
{
	terrain_Modelview = modelview;
	terrain_Projection = projection;

	glUseProgram(shaderProgram);
	// Apply model view transformations	
	//NEVER MESS WITH THIS!!
	terrain_Modelview = glm::translate(terrain_Modelview, terrainPosition);
	terrain_Modelview = glm::scale(terrain_Modelview, glm::vec3(terrainScale));
	//terrain_Modelview = glm::rotate(terrain_Modelview, terrainAngle, glm::vec3(terrainRotX,terrainRotY,terrainRotZ));

	//shaderM.setShaderUniforms(shaderProgram, terrain_Modelview, terrain_Projection, true);

	this->renderTerrain(GL_TRIANGLES, Texture);

	terrain_Modelview = modelview;
}

void Terrain::CheckPlayerPosition(glm::vec3 playerPosition)
{
	if (playerPosition.z > this->north && playerPosition.z < this->south &&
		playerPosition.x > this->west && playerPosition.x < this->east) {
		ActiveTerrain = true;
	}
	else
		ActiveTerrain = false;

	if (ActiveTerrain) {

		if (playerPosition.z < this->north + detectionDistance) {
			if (northHasBeenGenerated == false) {
				generateNorthTerrain = true;
				northHasBeenGenerated = true;
			}
			else
				generateNorthTerrain = false;
		}

		if (playerPosition.z > this->south - detectionDistance) {
			if (southHasBeenGenerated == false) {
				generateSouthTerrain = true;
				southHasBeenGenerated = true;
			}
			else
				generateSouthTerrain = false;
		}


		if (playerPosition.x < this->west + detectionDistance) {
			if (westHasBeenGenerated == false) {
				generateWestTerrain = true;
				westHasBeenGenerated = true;
			}
			else
				generateWestTerrain = false;
		}


		if (playerPosition.x > this->east - detectionDistance) {
			if (eastHasBeenGenerated == false) {
				generateEastTerrain = true;
				eastHasBeenGenerated = true;
			}
			else
				generateEastTerrain = false;
		}
	}
}

void Terrain::OutputPerlinValues()
{
	cout << "------:::: The Perlin values for this terrain ::::------" << endl;
	cout << "Frequency:         |" << perlinValues.frequency << endl;
	cout << "Persistence:       |" << perlinValues.persistence << endl;
	cout << "Noise:             |" << perlinValues.randomNoise << endl;
}

void Terrain::initializeGrid(int width, int height)
{
	//This will run through and create our grid points in 3D space
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			grid.push_back(glm::vec3(i, 0, j));
			gridNormals.push_back(normalize(glm::vec3(i, 0, j)));

			terrainUV.push_back(glm::vec2(i, j));
		}
	}

	for (int row = 0; row < height - 1; row++) {
		if ((row & 1) == 0) { // even rows
			for (int col = 0; col < width; col++) {
				indices.push_back(col + row * width);
				indices.push_back(col + (row + 1) * width);

			}
		}
		else { // odd rows
			for (int col = width - 1; col > 0; col--) {
				indices.push_back(col + (row + 1) * width);
				indices.push_back(col - 1 + +row * width);

			}
		}
	}

	glGenBuffers(1, &terrainVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, grid.size() * sizeof(glm::vec3), &grid[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);  //Enable attribute index 0 (position)


	glGenBuffers(1, &terrainNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, terrainNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, gridNormals.size() * sizeof(glm::vec3), &gridNormals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &terrainIndicesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIndicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//load into the texture buffer
	glGenBuffers(1, &terrainUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, terrainUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, terrainUV.size() * sizeof(glm::vec2), &terrainUV[0], GL_STATIC_DRAW);
}

void Terrain::RecalculateHeightMesh(std::vector<float> newYVertices)
{
	int result = terrainWidth * terrainHeight;
	//This will run through and create our grid points in 3D space based on new values from Perlin noise
	for (int i = 0; i < result; i++) {
		grid[i] = glm::vec3(grid[i].x, newYVertices[i] / 10.0f, grid[i].z);
		gridNormals[i] = normalize(grid[i]);

	}

	GenerateBoundingBox();

	glBindBuffer(GL_ARRAY_BUFFER, terrainVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, grid.size() * sizeof(glm::vec3), &grid[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIndicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);



	glBindBuffer(GL_ARRAY_BUFFER, terrainNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, gridNormals.size() * sizeof(glm::vec3), &gridNormals[0], GL_STATIC_DRAW);
}

void Terrain::renderTerrain(GLenum drawMode, GLuint texture)
{
	if (toggleWireframe) {
		drawMode = GL_LINE_LOOP;
	}
	else
		drawMode = GL_TRIANGLE_STRIP;

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// 1st attribute buffer : vertices

	glBindBuffer(GL_ARRAY_BUFFER, terrainVertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);


	// 2nd attribute buffer : Normals

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, terrainNormalBuffer);
	glVertexAttribPointer(
		1,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : UVs

	glBindBuffer(GL_ARRAY_BUFFER, terrainUVBuffer);
	glVertexAttribPointer(
		2,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	glEnableVertexAttribArray(2);



	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIndicesBuffer);

	// Draw the triangles !
	glDrawElements(
		drawMode,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);
}

void Terrain::GenerateBoundingBox()
{
	front_bottom_left = grid[0];
	back_top_right = grid[0];

	for (int i = 0; i < grid.size(); i++) {
		//front bottom left most point
		if (grid[i].x < front_bottom_left.x)
			front_bottom_left.x = grid[i].x;

		if (grid[i].y < front_bottom_left.y)
			front_bottom_left.y = grid[i].y;

		if (grid[i].z < front_bottom_left.z)
			front_bottom_left.z = grid[i].z;

		//back top right most point
		if (grid[i].x > back_top_right.x)
			back_top_right.x = grid[i].x;

		if (grid[i].y > back_top_right.y)
			back_top_right.y = grid[i].y;

		if (grid[i].z > back_top_right.z)
			back_top_right.z = grid[i].z;

	}

	//create vertices for box around object
	box_vertices.push_back(front_bottom_left);
	box_vertices.push_back(glm::vec3(back_top_right.x, front_bottom_left.y, front_bottom_left.z));	//front bottom right
	box_vertices.push_back(glm::vec3(front_bottom_left.x, back_top_right.y, front_bottom_left.z));	//front top left
	box_vertices.push_back(glm::vec3(back_top_right.x, back_top_right.y, front_bottom_left.z));		//front top right

	box_vertices.push_back(back_top_right);
	box_vertices.push_back(glm::vec3(front_bottom_left.x, back_top_right.y, back_top_right.z));		//back top left
	box_vertices.push_back(glm::vec3(front_bottom_left.x, front_bottom_left.y, back_top_right.z));	//back bottom left
	box_vertices.push_back(glm::vec3(back_top_right.x, front_bottom_left.y, back_top_right.z));		//back bottom right

	glGenBuffers(1, &boxBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, boxBuffer);
	glBufferData(GL_ARRAY_BUFFER, box_vertices.size() * sizeof(glm::vec3), &box_vertices[0], GL_STATIC_DRAW);

	south = (this->box_vertices[4].z * terrainScale) + this->terrainPosition.z;
	north = (this->box_vertices[3].z * terrainScale) + this->terrainPosition.z;

	west = (this->box_vertices[0].x * terrainScale) + this->terrainPosition.x;
	east = (this->box_vertices[4].x * terrainScale) + this->terrainPosition.x;
}

void Terrain::DrawBoundingBox()
{
	// glLineWidth(3.0);
	glBindBuffer(GL_ARRAY_BUFFER, boxBuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINE_LOOP, 0, box_vertices.size());
}
