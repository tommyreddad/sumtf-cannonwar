#include "entity.hpp"

extern bool GameOver;

std::list<Entity*> Entities;
std::list<Entity*> DestroyedEntities;

/* * * * * * * * * * * * *
 *    ENTITY BEHAVIOUR   *
 * * * * * * * * * * * * */

/* The cannon ball entity's data */
CannonBall::CannonBall()
{
	collidedGround = false;
	m = 10.0;
	r = 4.0;
	e = 0.2;
	f = 0.5;
	a = 0.1;
	affected = true;
}

/* The first time the cannon ball collides with the ground, we stop its motion */
void CannonBall::onGround(Entity *e, sf::Vector2<double> n)
{
	if (!collidedGround) {
		collidedGround = true;
		e->v.x = 0.0;
		e->v.y = 0.0;
	}
}

/* Destroy if the cannonball exits the world bounds (i.e. offscreen or the river) */
void CannonBall::onBounds(Entity *e)
{
	e->destroy(); 
}

/* The cannon entity data */
Cannon::Cannon()
{
	w = 8;
	h = 20;
	affected = false;
	power = 5;
	delay = 0.0f;
}

/* Game over if the entity hits a cannon directly. */
void Cannon::onCollision(Entity *e, Entity *f)
{
	CannonBall *c = dynamic_cast<CannonBall*>(f->ed);
	if (c != 0) {
		if (!c->collidedGround && e->t != f->t) {
			GameOver = true;
		}
	}
}

void Cannon::up(Entity *e)
{
	e->theta += ANGLE_INCREMENT*Pi/180.0;
}

void Cannon::down(Entity *e)
{
	e->theta -= ANGLE_INCREMENT*Pi/180.0;
}

/* * * * * * * * * * * * */

/* Generic draw function to allow for multiple different types of entities. */
void Entity::draw()
{
	ed->onDraw(this);
}

/* We only provide one entity constructor: with initial position specified. */
Entity::Entity(EntityData *data, Team team, double x, double y, double angle)
{
	ed = data;
	t = team;
	p.x = x;
	p.y = y;
	theta = angle;
	Entities.push_back(this);
}

/* Must destroy entities using this method. */
void Entity::destroy()
{
	DestroyedEntities.push_back(this);
}

