#include "perlin.hpp"

using namespace std;

/* A pseudorandom noise function credited to Hugo Elias. Generates a float in [-1, 1] */
float noise(int x)
{
	x = (x << 13)^x;
	return (1.0 - ((x*(x*x*15731 + 789221) + 1376312589) & 0x7fffffff)/1073741824.0);
}

/* Interpolating x in [a, b] using S(x) = 3x^2 - 2x^3 */
float interpolate(float a, float b, float x)
{
	float f = x*x*(3.0 - 2.0*x);
	return a*(1.0 - f) + b*f;
}

/* Smooth noise function */
float smoothNoise(int x)
{
	return noise(x)/2.0 + noise(x - 1)/4.0 + noise(x + 1)/4.0;
}

/* Coherent noise */
float coherentNoise(float x)
{
	int i = (int)x;
	return interpolate(smoothNoise(i), smoothNoise(i + 1), x - (float)i);
}

/* Perlin noise function at x */
float Perlin::perlinNoiseAt(float x)
{
	float total = 0.0, frequency = 1.0, amplitude = map.amplitude_init;
	for (int i = 0; i < map.octaves; i++) {
		frequency *= 2.0;
		amplitude *= map.persistence;
		total += coherentNoise((x - map.shift)*frequency)*amplitude;
	}
	return total;
}

/** Store the full array of Perlin noise values into terrain capped between min and max */
int* Perlin::computePerlinNoise(int min, int max)
{
	float F[map.width];
	float pMin = 0, pMax = 0;
  
	// We first interpolate through the points provided on the map layout
	list<sf::Vector2i>::iterator it = map.points.begin();
	sf::Vector2i crt;
	sf::Vector2i prev = *it;
	if (prev.x > 0) {
		for (int j = 0; j < prev.x; j++)
			terrain[j] = prev.y;
	}
	for (; it != map.points.end(); it++) {
		crt = *it;
		if (crt.x == 0)
			continue;
		for (int j = prev.x; j < crt.x; j++)
			terrain[j] = interpolate(prev.y, crt.y, (float)(j - prev.x)/(float)(crt.x - prev.x));
		prev = crt;
	}
	if (crt.x < map.width - 1) {
		for (int j = crt.x; j < map.width; j++)
			terrain[j] = crt.y;
	}      
  
	// Then add the perlin noise values to this
	for (int i = 0; i < map.width; i++) {
		F[i] = perlinNoiseAt((float)i/(float)map.width);
		if (F[i] < pMin)
			pMin = F[i];
		else if (F[i] > pMax)
			pMax = F[i];
	}

	// Scale the final values so they fit within bounds
	for (int i = 0; i < map.width; i++) {
		terrain[i] += (int)((max - min)*(F[i] - pMin)/(pMax - pMin)) + min;
	}

	return terrain;
}

Perlin::Perlin(Map m)
{
	map = m;
	terrain = new int[map.width];
}

Perlin::~Perlin()
{
	delete terrain;
}

