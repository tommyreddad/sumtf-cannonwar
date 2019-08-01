#include "physics.hpp"

using namespace std;

sf::Vector2<double> Zero(0.0, 0.0);
sf::Vector2<double> Gravity(0.0, GRAVITY);

/* Just initializing the physics environment */
Physics::Physics(int *t, int mx, int Mx, int my, int My, double m)
{
	terrain = t;
	minx = mx;
	maxx = Mx;
	miny = my;
	maxy = My;
	windMax = m;
}

Physics::~Physics()
{
	for (list<Entity*>::iterator it = Entities.begin(); it != Entities.end(); it++) {
		(*it)->destroy();
	}
	Entities.clear();
	update(0);
}

/* Wind updating function. Randomly updates the coordinates of the wind vector. */
void Physics::windUpdate(double t)
{
	wind.x += t*(double)(rand()%WINDVOLATILITY - rand()%WINDVOLATILITY);
	wind.y += t*(double)(rand()%WINDVOLATILITY - rand()%WINDVOLATILITY);
  
	// Normalize the wind vector if it's too powerful
	double w = dot(wind, wind);
	if (w >= windMax*windMax)
		wind *= 1.0/sqrt(w);
}

/* Generic dot product function. Apparently not provided with SFML2 (yet) */
double dot(sf::Vector2<double> v, sf::Vector2<double> u)
{
	return v.x*u.x + v.y*u.y;
}

/* Update the entity positions. Useful for collision resolution */
void Physics::move(double t)
{
	for (list<Entity*>::iterator it = Entities.begin(); it != Entities.end(); it++) {
		Entity* e = *it;
		if (e->ed->affected)
			e->p = e->p + t*e->v;
	}
}

/* Move an entity away from the ground. Uses a pseudo-binary search technique */
void Physics::moveApartGround(double t, Entity *e)
{
	int N = 10;
	sf::Vector2<double> p = e->p;

	t = -t;
	for (int i = 0; i < N; i++) {
		t /= 2.0;
		e->p = e->p + t*e->v;
		if (isCollidingGround(e) == Zero)
			t = abs(t);
		else
			t = -abs(t);
	}
	e->p = p;
	move(t);
}

/* Similar binary search technique for moving two entities apart */
void Physics::moveApart(double t, Entity *e, Entity *f)
{
	int N = 5;
	sf::Vector2<double> p = e->p;
	sf::Vector2<double> q = f->p;

	t = -t;
	for (int i = 0; i < N; i++) {
		t /= 2.0;
		e->p = e->p + t*e->v;
		f->p = f->p + t*f->v;
		if (areColliding(e, f) == Zero)
			t = abs(t);
		else
			t = -abs(t);
	}
	e->p = p;
	f->p = q;
	move(t);
}

/* Simply verify that two entities are currently colliding */
sf::Vector2<double> Physics::areColliding(Entity *e, Entity *f)
{
	CannonBall *p = dynamic_cast<CannonBall*>(e->ed);
	CannonBall *q = dynamic_cast<CannonBall*>(f->ed);
	if (p == 0 && q == 0) {
		// Ignore the collision if both are rectangles
		return Zero;
	} else if (p != 0 && q != 0) {
		// Both are circles
		sf::Vector2<double> n = e->p - f->p;
		double d = dot(n, n);
		if (d <= (p->r + q->r)*(p->r + q->r))
			return n/sqrt(d);
		return Zero;
	} else {
		// Setting the pointers properly to avoid code copying
		Cannon* r;
		if (p == 0) {
			Entity* tmp;
			r = dynamic_cast<Cannon*>(e->ed);
			tmp = f;
			f = e;
			e = tmp;
			p = q;
		} else {
			r = dynamic_cast<Cannon*>(f->ed);
		}
    
		// At this point, (r, f) is the rectangle and (p, e) is the circle
		// Verify if the center of the circle is inside the rectangle
		double a = f->p.x + (r->w/2)*cos(f->theta + Pi/2.0);
		double b = f->p.x + (r->w/2)*cos(f->theta - Pi/2.0);
		double c = f->p.y + (r->w/2)*sin(f->theta + Pi/2.0);
		double d = f->p.y + (r->w/2)*sin(f->theta - Pi/2.0);
      
		sf::Vector2<double> p1(a - (r->h/2)*cos(f->theta), c - (r->h/2)*sin(f->theta));
		sf::Vector2<double> p2(a + (r->h/2)*cos(f->theta), c + (r->h/2)*sin(f->theta));
		sf::Vector2<double> p3(b + (r->h/2)*cos(f->theta), d + (r->h/2)*sin(f->theta));
		sf::Vector2<double> p4(b - (r->h/2)*cos(f->theta), d - (r->h/2)*sin(f->theta));

		sf::Vector2<double> dp1 = p2 - p1;
		sf::Vector2<double> dp2 = p4 - p1;

		double d1 = dot(e->p - p1, dp1)/dot(dp1, dp1);
		double d2 = dot(e->p - p1, dp2)/dot(dp2, dp2);

		if (d1 >= 0 && d1 <= 1 && d2 >= 0 && d2 <= 1) {
			// Circle is contained in the rectangle
			return sf::Vector2<double>(1.0, 0.0);
		}
    
		// Otherwise, we check for each possible line segment intersection 
		if (d1 >= 0 && d1 <= 1) {
			p3 = e->p - (p1 + d1*dp1);
			if (dot(p3, p3) <= p->r*p->r)
				return p3/sqrt(dot(p3, p3));
			else {
				p3 = e->p - (p4 + d1*dp1);
				if (dot(p3, p3) <= p->r*p->r)
					return p3/sqrt(dot(p3, p3));
			}
		}
		if (d2 >= 0 && d2 <= 1) {
			p3 = e->p - (p1 + d2*dp2);
			if (dot(p3, p3) <= p->r*p->r)
				return p3/sqrt(dot(p3, p3));
			else {
				p3 = e->p - (p2 + d2*dp2);
				if (dot(p3, p3) <= p->r*p->r)
					return p3/sqrt(dot(p3, p3));
			}
		}
	}

	return Zero;
}

/* Return the time of collision between two entities if they collide within the
 * next frame. Otherwise, return -1.0 */
double Physics::willCollide(double t, Entity *e, Entity *f)
{
	CannonBall *p = dynamic_cast<CannonBall*>(e->ed);
	CannonBall *q = dynamic_cast<CannonBall*>(f->ed);
  
	if (p == 0 || q == 0) {
		// Ignore the collision if one is a rectangle
		return -1.0;
	}

	sf::Vector2<double> dp = f->p - e->p;
	sf::Vector2<double> dv = f->v - e->v;

	// We solve the quadratic equation |dp + dv*t| = r
	double d = dot(dp, dv);
	double v = dot(dv, dv);
	double r = p->r + q->r;
	double s = d*d - v*(dot(dp, dp) - r*r);
	// A solution only exists if the discriminant is positive
	if (s >= 0) {
		s = (-d - sqrt(s))/v;
		// We only return forward collisions within the next frame
		if (s >= 0.0 && s <= t)
			return s;
	}
	return -1.0;
}

/* Return the terrain normal if the entity is colliding with the terrain. Otherwise,
 * return Zero. */
sf::Vector2<double> Physics::isCollidingGround(Entity *e)
{
	CannonBall *p = dynamic_cast<CannonBall*>(e->ed);

	if (p == 0) {
		// Ignore ground collision if not a circle
		return Zero;
	}
  
	double d;
	bool groundCollided = false;
	sf::Vector2<double> n(0.0, 0.0);
	sf::Vector2<double> m(0.0, 0.0);
	for (int i = 0; i <= (int)(p->r); i++) {
		if ((int)(e->p.x) + i >= maxx || (int)(e->p.x) - i < minx) {
			continue;
		}
		d = e->p.y - sqrt(p->r*p->r - i*i);
		if (d <= terrain[(int)(e->p.x) + i]) {
			// collided on the right
			m.y = e->p.y - terrain[(int)(e->p.x) + i];
			if (m.y <= 0.0) {
				m.y = 0.0;
				m.x = -1.0;
			} else
				m.x = -i;
			if (m.x != 0 || m.y != 0) {
				n = n + m/sqrt(dot(m, m));
				groundCollided = true;
			}
		}
		if (i != 0) {
			if (d <= terrain[(int)(e->p.x) - i]) {
				// collided on the left
				m.y = e->p.y - terrain[(int)(e->p.x) - i];
				if (m.y <= 0.0) {
					m.y = 0.0;
					m.x = 1.0;
				} else
					m.x = i;
				if (m.y != 0) {
					n = n + m/sqrt(dot(m, m)); 
					groundCollided = true;
				}
			}
		}
	}
	if (groundCollided)
		return n/sqrt(dot(n, n));
	return Zero;
}

/* Main update function for the physics system. */
void Physics::update(double t)
{
	Entity *e, *f;
	sf::Vector2<double> n;
	double s;

	windUpdate(t);

	// Move the entities, apply impulse and forces
	for (list<Entity*>::iterator it = Entities.begin(); it != Entities.end(); it++) {
		Entity* e = *it;

		// Update position and velocity
		if (e->ed->affected) {
			e->p += t*e->v;
			e->v += t*((1.0/e->ed->m)*(wind - e->ed->a*e->v) - Gravity);
		}
	}

	// Test for collision
	for (list<Entity*>::iterator it1 = Entities.begin(); it1 != Entities.end(); it1++) {
		e = *it1;

		// Test for out of bounds
		if (e->p.x < minx || e->p.x >= maxx || e->p.y < miny || e->p.y >= maxy) {
			e->ed->onBounds(e);
		}

		// Test for ground collision.
		if (e->ed->affected && (n = isCollidingGround(e)) != Zero) {
			// Apply the impulse for collision resolution, if moving towards the ground
			if (dot(n, e->v) < 0.0) {
				e->v += (1.0 + e->ed->e)*abs(dot(n, e->v))*n;

				// Project the gravitational force on the terrain normal
				double d = e->ed->f*e->ed->m*dot(Gravity, n);
				if (d*d <= dot(e->v, e->v))
					e->v -= d*e->v/sqrt(dot(e->v, e->v));
			}

			// Move the entity so that it's nearly touching the ground
			moveApartGround(t, e);

			e->ed->onGround(e, n);
		}

		// Test against all other entities
		for (list<Entity*>::iterator it2 = ++it1; it2 != Entities.end(); it2++) {
			f = *it2;
      
			if ((s = willCollide(t, e, f)) >= 0.0) {
				// Move the entities so that they're just touching
				move(s);
				n = e->p - f->p;
				n *= 1/sqrt(dot(n, n));
        
				// Apply impulses for collision resolution
				if (e->ed->affected && f->ed->affected) {
					e->v +=  (1.0 + e->ed->e)*abs(dot(n, e->v))*n;
					f->v += -(1.0 + f->ed->e)*abs(dot(n, f->v))*n;
				}
        
				e->ed->onCollision(e, f);
				f->ed->onCollision(f, e);
			} else if ((n = areColliding(e, f)) != Zero) {
				moveApart(t, e, f);
        
				if (e->ed->affected && f->ed->affected) {
					e->v +=  (1.0 + e->ed->e)*abs(dot(n, e->v))*n;
					f->v += -(1.0 + f->ed->e)*abs(dot(n, f->v))*n;
				}

				e->ed->onCollision(e, f);
				f->ed->onCollision(f, e);
			}
		}
		it1--;
	}

	// Remove all destroyed entities
	while (!DestroyedEntities.empty()) {
		e = DestroyedEntities.back();
		DestroyedEntities.pop_back();
		Entities.remove(e);
		delete e->ed;
		delete e;
	}

}
