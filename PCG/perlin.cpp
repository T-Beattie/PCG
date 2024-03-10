#include "perlin.h"


Perlin::Perlin()
{
}

void Perlin::GeneratePerlinNoise(float baseFreq, float persistence, int w, int h, float randomValue, int offsetX, int offsetY)
{
	width = w;
	height = h;

	printf("Generating noise texture...");

	data = new GLubyte[width * height * 4];

	double xRange = 1.0;
	double yRange = 1.0;
	double xFactor = xRange / width;
	double yFactor = yRange / height;

	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			float x = xFactor * (i + offsetX);
			float y = yFactor * (j + offsetY);
			float sum = 0.0f;
			float freq = baseFreq;
			float persist = persistence;
			for (int oct = 0; oct < 1; oct++) {
				glm::vec3 p(x * freq, y * freq, randomValue);

				float val = glm::perlin(p) * persist;

				sum += val;

				float result = sum + 0.5;

				// Clamp strictly between 0 and 1
				result = result > 1.0f ? 1.0f : result;
				result = result < 0.0f ? 0.0f : result;

				// Generate Turbulence
				result *= turbulence(p, 32.0f);

				// Store in texture
				data[((j * width + i) * 4) + oct] = (GLubyte)(result * 255.0f);

				averageVector.push_back(data[((j * width + i) * 4) + oct]);

				freq *= 2.0f;
				persist *= persistence;
			}
		}
	}

	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//And done. 
	delete[] data;
}

GLubyte* Perlin::getPerlinData()
{
	return data;
}

void Perlin::GrabEdgeValues()
{
	for (int i = 0; i < width; i++) {

		northEdge.push_back(data[((i * width) + 0) * 4]);
		southEdge.push_back(data[((i * width) + height) * 4]);
		eastEdge.push_back(data[((height * width) + i) * 4]);
		westEdge.push_back(data[((0 * width) + 1) * 4]);

	}
}

float Perlin::turbulence(glm::vec3 point, float f)
{
	double t = -.5;
	for (; f <= width / 12; f *= 2)
		t += abs(glm::perlin(glm::vec4(point.x, point.y, point.z, f)) * f);
	return t;
}