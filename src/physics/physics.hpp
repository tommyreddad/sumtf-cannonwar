#ifndef PHYSICS_H
#define PHYSICS_H

#define DAMPING 0.99
#define WINDVOLATILITY 200
#define GRAVITY 150.0

#include <list>
#include <cmath>
#include <cstdlib>

#include "entity.hpp"
#include "../perlin/map.hpp"

extern std::list<Entity*> Entities, DestroyedEntities;

double dot(sf::Vector2<double> v, sf::Vector2<double> u);

class Physics {
public:
	sf::Vector2<double> wind;
	double windMax; // max wind force

	int minx, maxx, miny, maxy; // physical world bounds
	int *terrain;
    
	Physics(int* t, int mx, int Mx, int my, int My, double m);
	~Physics();

	void windUpdate(double t);
	void move(double t);
	void moveApart(double t, Entity *e, Entity *f);
	void moveApartGround(double t, Entity* e);
	void update(double t);
	sf::Vector2<double> areColliding(Entity *e, Entity *f);
	sf::Vector2<double> isCollidingGround(Entity *e);
	double willCollide(double t, Entity *e, Entity *f);
};

#endif
