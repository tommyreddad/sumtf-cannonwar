#include "game.hpp"

bool GameOver = false;

double const Pi = 4.0*atan(1.0);

using namespace std;
using namespace sf;

Perlin *PerlinInstance;
Physics *World;
Cannon *PlayerData, *EnemyData;
Entity *Player, *Enemy;

/* Draw an entity as a circle. */
void CannonBall::onDraw(Entity *e)
{
	int N = 10;
	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_POLYGON);
	for(int i = 0; i < N; i++) {
		glVertex2f(e->p.x + r*cos(2*Pi*i/N), e->p.y + r*sin(2*Pi*i/N));
	}
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
}

/* Draw a cannon as an angled rectangle. */
void Cannon::onDraw(Entity *e)
{
	double x1 = e->p.x + (w/2)*cos(e->theta + Pi/2.0);
	double x2 = e->p.x + (w/2)*cos(e->theta - Pi/2.0);
	double y1 = e->p.y + (w/2)*sin(e->theta + Pi/2.0);
	double y2 = e->p.y + (w/2)*sin(e->theta - Pi/2.0);
	glBegin(GL_POLYGON);
	  glVertex2f(x1 - (h/2)*cos(e->theta), y1 - (h/2)*sin(e->theta));
	  glVertex2f(x1 + (h/2)*cos(e->theta), y1 + (h/2)*sin(e->theta));
	  glVertex2f(x2 + (h/2)*cos(e->theta), y2 + (h/2)*sin(e->theta));
	  glVertex2f(x2 - (h/2)*cos(e->theta), y2 - (h/2)*sin(e->theta));
	glEnd();
}

/* Draw the Perlin noise generated terrain */
void Perlin::draw()
{
	glBegin(GL_QUADS);
	for (int i = 0; i < map.width - 1; i++) {
		glVertex2f(i, 0);
		glVertex2f(i + 1, 0);
		glVertex2f(i + 1, terrain[i + 1]);
		glVertex2f(i, terrain[i]);
	}
	glEnd();
}

/* Draws a simple arrow from (x1, y1) to (x2, y2). Used for the wind indicator. */
void drawArrow(float x1, float y1, float x2, float y2)
{
	double a = atan2(y1 - y2, x1 - x2);
	glBegin(GL_LINES);
	  glVertex2f(x1, y1);
	  glVertex2f(x2, y2); 
	  glVertex2f(x2, y2);
	  glVertex2f(x2 + 4.5*cos(a + Pi/4.0), y2 + 4.5*sin(a + Pi/4.0));
	  glVertex2f(x2, y2);
	  glVertex2f(x2 + 4.5*cos(a - Pi/4.0), y2 + 4.5*sin(a - Pi/4.0));
	glEnd();
}

void render(Map map)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Draw entities
	for (list<Entity*>::iterator it = Entities.begin(); it != Entities.end(); it++)
		(*it)->draw();

	// Draw the river
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
	  glVertex2f(0, 0);
	  glVertex2f(map.width, 0);
	  glVertex2f(map.width, 100);
	  glVertex2f(0, 100);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);

	// Draw terrain
	PerlinInstance->draw();
  
	// Draw UI last.
	// Draw power bar at the top left corner of the screen
	glLineWidth(8.0f);
	glBegin(GL_LINES);
	  glColor3f(0.5f, 0.5f, 0.5f);
	  glVertex2f(10, map.height - 10);
	  glVertex2f(10 + 100, map.height - 10);
	  glColor3f(0.5f, 0.0f, 0.0f);
	  glVertex2f(10, map.height - 10);
	  glVertex2f(10 + 10*PlayerData->power, map.height - 10);
	glEnd();
	glLineWidth(1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);

	// Draw the wind vector at the top right of the screen
	if (World->windMax > 0.0) {
		Vector2<double> w = World->wind;
		double s = sqrt(dot(w, w));
		glLineWidth(1.0f + 3.0f*s/World->windMax);
		w *= 15.0/(s + 0.01);
		drawArrow(map.width - 20, map.height - 20, map.width - 20 + w.x, map.height - 20 + w.y);
		glLineWidth(1.0f);
	}
}

int main(int argc, const char *argv[])
{
	Map map;
  
	// Recovering the maps directory
	string mapdir = string(argv[0]);
	mapdir = mapdir.substr(0, 1 + mapdir.substr(0, mapdir.length()).find_last_of("/")) + MAP_DIR + string("/");

	// Parsing the arguments
	bool demoMode = false;
	for (int i = 1; i < argc; i++) {
		if (string("-m").compare(argv[i]) == 0) {
			if (++i >= argc) {
				cerr << "No map specified." << endl;
				exit(1);
			} else {
				map.load(mapdir + string(argv[i]));
			}
		}
		if (string("-d").compare(argv[i]) == 0) {
			demoMode = true;
			cerr << "Demonstration mode." << endl;
		}
	}

	// Load the default map if no map was given
	if (map.path.compare("") == 0) {
		map.load(mapdir + DEFAULT_MAP);
	}
  
	// Load the perlin object
	PerlinInstance = new Perlin(map);
	int *terrain = PerlinInstance->computePerlinNoise(0, map.height/10);
 
	// Finding the first point ~2/3 of the way up the mountain from the left
	int m = 0;
	for (int i = 0; i < map.width; i++) {
		if (terrain[i] > m)
			m = terrain[i];
		if (terrain[i] <= 2*m/3) {
			m = i;
			break;
		}
	}
	PlayerData = new Cannon;
	Player = new Entity(PlayerData, PLAYER, m, terrain[m], Pi/4.0);

	// Finding the first point ~2/3 of the way up the mountain from the right
	m = 0;
	for (int i = map.width - 1; i >= 0; i--) {
		if (terrain[i] > m)
			m = terrain[i];
		if (terrain[i] <= 2*m/3) {
			m = i;
			break;
		}
	}
	EnemyData = new Cannon;
	Enemy = new Entity(EnemyData, ENEMY, m, terrain[m], 3.0*Pi/4.0);

	// Initialize the random seed
	srand(time(NULL));

	// Initializing the render window. Super easy with SFML!
	RenderWindow window(VideoMode(map.width, map.height, 32), TITLE);
	window.setFramerateLimit(60);

	// Setting up OpenGL for 2D drawing
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, map.width, 0, map.height, -1, 1);

	glDisable(GL_DEPTH_TEST);

	// Clock to deal with uneven frames
	Clock clock;
	float t;

	// Initialize the physics engine. Give 0 wind speed if in demo mode
	if (demoMode)
		World = new Physics(terrain, 0, map.width, 80, map.height, 0.0);
	else 
		World = new Physics(terrain, 0, map.width, 80, map.height, 100.0);

	// Find the required speed to hit the other cannon from 45 degree aoa
	float dx = Enemy->p.x + (EnemyData->h/2)*cos(Enemy->theta) -
		Player->p.x - (PlayerData->h/2)*cos(Player->theta);
	float dy = Enemy->p.y + (EnemyData->h/2)*sin(Enemy->theta) -
		Player->p.y - (PlayerData->h/2)*sin(Player->theta);
	double reqSpeed = dx/sqrt((dx - dy)/GRAVITY);

	if (demoMode) {
		float timeout = 0;

		// First demonstration: ignoring cannon collision
		cerr << "Demonstration 1: ignoring cannon collision" << endl;
    
		Entity* c = new Entity(new CannonBall, PLAYER,
				       Player->p.x + (PlayerData->h/2)*cos(Player->theta),
				       Player->p.y + (PlayerData->h/2)*sin(Player->theta), 0.0);
      
		// We fire the entity so that it hits the ground right above the enemy cannon
		c->v.x = (6.0/5.0)*reqSpeed*cos(Player->theta);
		c->v.y = (6.0/5.0)*reqSpeed*sin(Player->theta);
    
		// Wait until the cannonball is destroyed to continue to the next test
		while (window.isOpen()) {
			t = clock.getElapsedTime().asSeconds();
			clock.restart();
			timeout += t;

			bool b = false;
			for (list<Entity*>::iterator it = Entities.begin(); it != Entities.end(); it++) {
				if ((*it) == c) {
					b = true;
					break;
				}
			}
			if (!b)
				break;

			World->update(t);
			render(map);
			window.display();

			if (timeout > 5)
				break;
		}

		timeout = 0;

		// Second demonstration: cannonball-to-cannonball collisions
		cerr << "Demonstration 2: cannonball-to-cannonball collision" << endl;
    
		c = new Entity(new CannonBall, PLAYER, map.width/2 - 100, map.height/2 + 100, 0.0);
		c->v.x = 200;
		c->v.y = 50;

		Entity* d = new Entity(new CannonBall, ENEMY, map.width/2 + 100, map.height/2 + 100, 0.0);
		d->v.x = -200;
		d->v.y = 50;
     
		// Wait until both cannonballs are destroyed to continue to the next test
		while (window.isOpen()) {
			t = clock.getElapsedTime().asSeconds();
			clock.restart();
			timeout += t;
        
			bool b = false;
			for (list<Entity*>::iterator it = Entities.begin(); it != Entities.end(); it++) {
				if ((*it) == c || (*it) == d) {
					b = true;
					break;
				}
			}
			if (!b)
				break;

			World->update(t);
			render(map);
			window.display();

			if (timeout > 5)
				break;
		}

		timeout = 0;

		// Third demonstration: game over on cannonball-to-cannon collision
		cerr << "Demonstration 3: game over on a direct cannonball-to-cannon collision" << endl;
    
		c = new Entity(new CannonBall, PLAYER,
			       Player->p.x + (PlayerData->h/2)*cos(Player->theta),
			       Player->p.y + (PlayerData->h/2)*sin(Player->theta), 0.0);
      
		// Shoot the cannonball at the required speed
		c->v.x = reqSpeed*cos(Player->theta);
		c->v.y = reqSpeed*sin(Player->theta);
          
		// Wait until game over to quit the demonstration
		while (window.isOpen()) {
			t = clock.getElapsedTime().asSeconds();
			clock.restart();
			timeout += t;
        
			if (GameOver)
				break;

			World->update(t);
			render(map);
			window.display();

			if (timeout > 5)
				break;
		}

		window.close();
    
		delete World;
		delete PerlinInstance;

		return EXIT_SUCCESS;
	}

	Event event;
	while (window.isOpen()) {
		if (GameOver) {
			Text gameOver;
			gameOver.setString("GAME OVER");
			gameOver.setCharacterSize(50);
			gameOver.setStyle(Text::Bold);
			gameOver.setColor(Color::Red);
			gameOver.setPosition(Vector2f(map.width/2 - gameOver.getLocalBounds().width/2,
						      map.height/2 - gameOver.getLocalBounds().height/2));

			window.draw(gameOver);
			window.display();

			do {
				window.pollEvent(event);
			} while (event.key.code != Keyboard::Escape);
      
			window.close();
			break;
		}

		t = clock.getElapsedTime().asSeconds();
		clock.restart();

		// Event handling
		while (window.pollEvent(event)) {
			// Handling the window close event
			if (event.type == Event::Closed)
				window.close();
			if (event.type == Event::KeyPressed) {
				// Close the window on ESCAPE
				if (event.key.code == Keyboard::Escape)
					window.close();
        
				// Player controls
				if (event.key.code == Keyboard::Up) {
					if (Player->theta < Pi/2.0)
						PlayerData->up(Player);
				}
				if (event.key.code == Keyboard::Down) {
					if (Player->theta > 0.0)
						PlayerData->down(Player);
				}
				if (event.key.code == Keyboard::Left) {
					if (PlayerData->power > 0)
						PlayerData->power -= 1;
				}
				if (event.key.code == Keyboard::Right) {
					if (PlayerData->power < 10)
						PlayerData->power += 1;
				}
				if (event.key.code == Keyboard::Space && PlayerData->delay <= 0.0) {
					// Spawn a cannonball
					Entity* c = new Entity(new CannonBall, PLAYER,
							       Player->p.x + (PlayerData->h/2)*cos(Player->theta),
							       Player->p.y + (PlayerData->h/2)*sin(Player->theta), 0.0);
          
					// Scale by 1/5 so that a power of 5/10 achieves the correct speed
					c->v.x = (1.0/5.0)*PlayerData->power*reqSpeed*cos(Player->theta);
					c->v.y = (1.0/5.0)*PlayerData->power*reqSpeed*sin(Player->theta);
          
					PlayerData->delay = DELAY;
				}
			}
		}
    
		// Decrement the delay to fire another cannonball
		if (PlayerData->delay > 0.0)
			PlayerData->delay -= t;

		// Update physics
		World->update(t);
 
		render(map);

		window.display();
	}

	World->update(0);
	delete World;
	delete PerlinInstance;

	return EXIT_SUCCESS;
}
