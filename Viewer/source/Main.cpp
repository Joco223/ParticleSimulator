#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
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
	std::vector<double> radii;
	std::vector<sf::ConvexShape> lines;
	std::vector<std::vector<Vec2D>> particles = loadFile(argv[1], ups, maxSteps, particleCount, radii, lines);
	if (particles.size() == 0) return -1;

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Particle Simulator Viewer");
	window.setFramerateLimit(60);

	sf::View camera = window.getDefaultView();

	sf::CircleShape particleCircle;
	particleCircle.setOrigin(sf::Vector2f(0, 0));
	particleCircle.setFillColor(sf::Color(255, 195, 54));
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

	sf::Shader metaBallsShader;
	metaBallsShader.loadFromFile("../source/MetaBalls.frag", sf::Shader::Fragment);
	sf::RectangleShape metaBallsRect;
	metaBallsRect.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(1, 1)));
	metaBallsRect.setPosition(sf::Vector2f(0, 0));
	metaBallsRect.setSize(sf::Vector2f(1280, 720));
	metaBallsShader.setUniform("ballCount", particleCount);
	sf::Glsl::Vec2 metaBallsPositions[particleCount];
	float ballRadii[particleCount];

	for (int i = 0; i < particleCount; i++) {
		ballRadii[i] = radii[i];
	}

	metaBallsShader.setUniformArray("metaBallsRadii", ballRadii, particleCount);

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

			double radius = radii[i];
			Vec2D newPos = (getParticlePos(currentStep, i, particles) - Vec2D(radius, radius));
			//newPos.x = 1280 - newPos.x;
			//sf::Vector2i converted = window.mapCoordsToPixel(sf::Vector2f(newPos.x, newPos.y));
			metaBallsPositions[i] = sf::Glsl::Vec2(newPos.x, newPos.y);
			//particleCircle.setPosition(newPos.x, newPos.y);
			//particleCircle.setRadius(radius);
			//window.draw(particleCircle);
		}

		metaBallsShader.setUniformArray("metaBalls", metaBallsPositions, particleCount);
		window.draw(metaBallsRect, &metaBallsShader);

		for (auto& i : lines) {
			window.draw(i);
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