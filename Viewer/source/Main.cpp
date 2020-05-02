#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include <cmath>

#include "Vec2D.h"
#include "Misc.h"



int main(int argc, char* argv[]) {

	if (argc < 2) {
		std::cerr << "No input file provided.\n";
		return -1;
	}

	int ups, maxSteps, particleCount;
	std::vector<std::vector<Vec2D>> particles = loadFile(argv[1], ups, maxSteps, particleCount);
	if (particles.size() == 0) return -1;

	sf::RenderWindow window(sf::VideoMode(800, 600), "Particle Simulator Viewer");
	window.setFramerateLimit(ups);

	sf::View camera = window.getDefaultView();

	sf::CircleShape particleCircle;
	particleCircle.setFillColor(sf::Color(255, 255, 255));
	particleCircle.setRadius(1);

	sf::Font font;
	font.loadFromFile("arial.ttf");

	sf::Text text;
	text.setFont(font);

	float currentStep = 0;
	float speed = 1.0f;
	float scale = 1.0f;
	float textScale = 1.0f;
	float oldSpeed = speed;

	std::vector<bool> keysPressed = {false, false, false, false}; //Left, Right, Up, Down

	while (window.isOpen()) {
		sf::Event Event;
		while (window.pollEvent(Event)) {
			if (Event.type == sf::Event::Closed) {
				window.close();
			}else if (Event.type == sf::Event::KeyPressed) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Period) && std::abs(speed) < 256) {
					speed *= 2;
				}else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Comma) && std::abs(speed) > 0.03125f) {
					speed /= 2;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::N) && scale < 16) {
					camera.zoom(2.0f);
					scale *= 2;
					textScale /= 2;
				}else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M) && scale > 0.03125f) {
					camera.zoom(0.5f);
					scale /= 2;
					textScale *= 2;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
					speed = -speed;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
					if (speed == 0) {
						speed = oldSpeed;
					}else{
						oldSpeed = speed;
						speed = 0;
					}
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) 
					keysPressed[0] = true;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
					keysPressed[1] = true;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
					keysPressed[2] = true;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
					keysPressed[3] = true;
			}else if (Event.type == sf::Event::KeyReleased) {	
				if (Event.key.code == sf::Keyboard::Left)
					keysPressed[0] = false;
				if (Event.key.code == sf::Keyboard::Right)
					keysPressed[1] = false;
				if (Event.key.code == sf::Keyboard::Up)
					keysPressed[2] = false;
				if (Event.key.code == sf::Keyboard::Down)
					keysPressed[3] = false;
			}
		}

		if (keysPressed[0]) {
			camera.move(sf::Vector2f(-3.0f, 0.0f)*scale);
		}else if (keysPressed[1]) {
			camera.move(sf::Vector2f(3.0f, 0.0f)*scale);
		}
		if (keysPressed[2]) {
			camera.move(sf::Vector2f(0.0f, -3.0f)*scale);
		}else if (keysPressed[3]) {
			camera.move(sf::Vector2f(0.0f, 3.0f)*scale);
		}

		window.setView(camera);
		window.clear();

		for (int i = 0; i < particleCount; i++) {
			Vec2D newPos = getParticlePos(currentStep, i, particles);
			particleCircle.setPosition(newPos.x, newPos.y);
			window.draw(particleCircle);
		}

		currentStep += speed;

		if (currentStep > maxSteps-1) { currentStep = 0; }

		if (currentStep < 0) { currentStep = maxSteps-1;}

		std::cout << currentStep << '/' << maxSteps << '\r';

		text.setString("Speed: " + std::to_string((int)std::floor(speed*100)) + "%\nScale: " + std::to_string((int)std::floor(textScale*100)) + "%\nTime: " + std::to_string(std::ceil((currentStep/ups)*100)/100).substr(0, 4) + "s/" + std::to_string((int)maxSteps/ups) + "s");
		window.setView(window.getDefaultView());
		window.draw(text);

		window.display();
	}
}