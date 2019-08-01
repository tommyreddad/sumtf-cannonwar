#ifndef MAP_H
#define MAP_H

#define MAP_DIR "maps"
#define DEFAULT_MAP "default.map"

#include <SFML/System.hpp>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <sstream>

class Map {
public:
	float persistence, amplitude_init, shift;
	int width, height, octaves;
	std::list<sf::Vector2i> points;
	std::string path;

	int load(std::string);
};

#endif
