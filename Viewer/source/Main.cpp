#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>

#include "Vec2D.h"

int main(int argc, char* argv[]) {

	if (argc < 2) {
		std::cerr << "No input file provided.\n";
		return -1;
	}

	std::ifstream input_file(argv[1], std::ios::in);
	if (!input_file.is_open()) {
		std::cerr << "Cannot open input file,\n";
		return -1;
	}

	int ups, maxSteps, particleCount;
	input_file.read((char*)&maxSteps, sizeof(int));
	input_file.read((char*)&particleCount, sizeof(int));
	input_file.read((char*)&ups, sizeof(int));

	std::vector<std::vector<Vec2D>> particles;

	for (int i = 0; i < maxSteps; i++) {
		std::vector<Vec2D> currentStep;

		for (int j = 0; j < particleCount; j++) {
			double x, y;
			input_file.read((char*)&x, sizeof(double));
			input_file.read((char*)&y, sizeof(double));
			currentStep.push_back(Vec2D(x, y));
		}

		particles.push_back(currentStep);
	} 

	sf::RenderWindow window(sf::VideoMode(800, 600), "Particle Simulator Viewer");
	window.setFramerateLimit(ups);
	sf::CircleShape particleCircle;
	particleCircle.setFillColor(sf::Color(255, 255, 255));
	particleCircle.setRadius(1);

	int currentStep = 0;

	while (window.isOpen()) {
		sf::Event Event;
		while (window.pollEvent(Event)) {
			if (Event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		for (auto& i : particles[currentStep]) {
			particleCircle.setPosition(i.x, i.y);
			window.draw(particleCircle);
		}

		currentStep++;

		if (currentStep > maxSteps) { currentStep = 0; }

		std::cout << currentStep << '/' << maxSteps << '\r';

		window.display();
	}
}