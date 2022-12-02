#include <SFML/Graphics.hpp>
#include <vector>

#define PI 3.14159265

const int GAME_WIDTH = 1920;
const int GAME_HEIGHT = 1080;
const int GAME_BOIDS_COUNT = 2000;
const float BOID_SIZE = 3;
const float BOID_SPEED = 0.5;
const float BOID_RANDOM_ROTATION_MAX_ANGLE = 0.5;
const float BOID_ROTATION_SPEED = 1;
const float BOID_ROTATION_TOLERANCE = 10;
const float BOID_COMMUNICATION_RADIUS = 100;

float distance(sf::Vector2f p1, sf::Vector2f p2)
{
	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) * 1.0);
}

float rand(int max)
{
	return std::rand() % max;
}

class Boid
{
public:
	sf::ConvexShape shape;
	float targetHeading;

	Boid(sf::Vector2f initPos, float initHeading)
	{
		targetHeading = initHeading;
		shape.setRotation(initHeading);
		shape.setPosition(initPos);

		shape.setPointCount(3);
		shape.setPoint(0, sf::Vector2f(-BOID_SIZE, BOID_SIZE));
		shape.setPoint(1, sf::Vector2f(BOID_SIZE, BOID_SIZE));
		shape.setPoint(2, sf::Vector2f(0, 5 * BOID_SIZE));
		shape.setFillColor(sf::Color::Black);
	}

	void update()
	{
		if (abs(shape.getRotation() - targetHeading) > BOID_ROTATION_TOLERANCE) { 
			if (shape.getRotation() - targetHeading > 0) {
				shape.rotate(-BOID_ROTATION_SPEED);
			}
			else {
				shape.rotate(BOID_ROTATION_SPEED);
			}
		}

		//shape.rotate(rand(10) / 10 - BOID_RANDOM_ROTATION_MAX_ANGLE);

		shape.move(
			-sin(shape.getRotation() * PI / 180) * BOID_SPEED,
			cos(shape.getRotation() * PI / 180) * BOID_SPEED
		);

		if (shape.getPosition().x > GAME_WIDTH) shape.setPosition(0, shape.getPosition().y);
		if (shape.getPosition().x < 0) shape.setPosition(GAME_WIDTH, shape.getPosition().y);
		if (shape.getPosition().y > GAME_HEIGHT) shape.setPosition(shape.getPosition().x, 0);
		if (shape.getPosition().y < 0) shape.setPosition(shape.getPosition().x, GAME_HEIGHT);
	}
};

std::vector<Boid> boids;

int main()
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	sf::RenderWindow window(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT, 32), "SFML",
		sf::Style::Titlebar | sf::Style::Close | sf::Style::Fullscreen);
	window.setVerticalSyncEnabled(true);

	for (int i = 0; i < GAME_BOIDS_COUNT; i++) {
		float heading = rand(360);
		float x = rand(GAME_WIDTH);
		float y = rand(GAME_HEIGHT);
		Boid boid(sf::Vector2f(x, y), heading);
		boids.push_back(boid);
	}

	sf::Clock clock;
	sf::Time previous;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
				break;
			}
		}

		window.clear(sf::Color::White);
			
		bool shouldCommunicate = false;
		if (clock.getElapsedTime().asMilliseconds() - previous.asMilliseconds() > 500) {
			previous = clock.getElapsedTime();
			shouldCommunicate = true;
		}

		for (int i = 0; i < GAME_BOIDS_COUNT; i++) {			
			if (shouldCommunicate) {
				for (int j = 0; j < GAME_BOIDS_COUNT; j++) {
					if (i != j && distance(boids.at(i).shape.getPosition(), boids.at(j).shape.getPosition()) < BOID_COMMUNICATION_RADIUS) {
						boids.at(i).targetHeading = boids.at(j).shape.getRotation();
						break;
					}
				}
			}
			boids.at(i).update();
			window.draw(boids.at(i).shape);
		}

		window.display();
	}

	return EXIT_SUCCESS;
}
