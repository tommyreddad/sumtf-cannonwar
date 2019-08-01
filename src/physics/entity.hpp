#ifndef ENTITY_H
#define ENTITY_H

#define ANGLE_INCREMENT 5

#include <SFML/System.hpp>
#include <list>

extern double const Pi;

enum Team {
	PLAYER, ENEMY
};

class EntityData;

/* Physical entity object. */
class Entity {
public:
	EntityData *ed;
	sf::Vector2<double> p, v;
	double theta;

	Team t;

	void draw();

	Entity(EntityData *data, Team team, double x, double y, double angle);
	void destroy();
};

/* Entity data object. Holds particular intrinsitc information about entity types. */
class EntityData {
public:
	double m; // mass
	double e; // coefficient of restitution
	double f; // coefficient of friction
	double a; // wind resistance

	bool affected; // affected by ground-entity collision

	/* Called for both (e, f) and (f, e) when a collision occurs. */
	virtual void onCollision(Entity *e, Entity *f) = 0;

	/* Called when a collision with the ground occurs. n is the normal */
	virtual void onGround(Entity *e, sf::Vector2<double> n) = 0;

	/* Called when the entity exits the physical world bounds. */
	virtual void onBounds(Entity *e) = 0;

	/* Allows us to specify a draw method for an entity type. */
	virtual void onDraw(Entity *e) = 0;
};


class CannonBall : public EntityData {
public:
	double r; // radius
    
	bool collidedGround;

	CannonBall();
	void onCollision(Entity *e, Entity *f) {}
	void onGround(Entity *e, sf::Vector2<double> n);
	void onBounds(Entity *e);
	void onDraw(Entity *e);
};

class Cannon : public EntityData {
public:
	double w, h;  // width and height

	int power;    // power between 0 and 10
	float delay;  // delay until the next available shot

	void up(Entity *e);
	void down(Entity *e);

	Cannon();
	void onCollision(Entity *e, Entity *f);
	void onGround(Entity *e, sf::Vector2<double> n) {}
	void onBounds(Entity *e) {}
	void onDraw(Entity *e);
};

#endif
