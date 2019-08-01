#ifndef GAME_H
#define GAME_H

#define TITLE "Super Ultra Mambo Tango Foxtrot Cannon War"

#define DELAY 0.7f

#include <GL/gl.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <ctime>
#include <unistd.h>
#include <cmath>

#include "perlin/map.hpp"
#include "perlin/perlin.hpp"
#include "physics/physics.hpp"
#include "physics/entity.hpp"

extern std::list<Entity*> Entities;

void drawArrow(float, float, float, float);

int main(int argc, const char* argv[]);

#endif
