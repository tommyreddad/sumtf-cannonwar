#ifndef PERLIN_H
#define PERLIN_H

#include <SFML/System.hpp>

#include "map.hpp"

float noise(int x);
float smoothNoise(int x);
float interpolate(float a, float b, float x);
float coherentNoise(float x);

class Perlin {
public:
	Map map;
    
	int *terrain;

	void draw();
	float perlinNoiseAt(float x);
	int *computePerlinNoise(int min, int max);
    
	Perlin(Map);
	~Perlin();
};

#endif
