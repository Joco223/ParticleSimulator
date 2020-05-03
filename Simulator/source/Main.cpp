#include "CfgParser.h"
#include "Particle.h"
#include "QuadTree.h"
#include "Vec2D.h"
#include "Physics.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>

std::vector<bool> required_config = {
	false, //UPS
	false, //ST
	false  //TF
};

int main(int argc, char** argv) {

	if (argc < 2) {
		std::cerr << "No config file provided.\n";
		return -1;
	}

	std::vector<std::pair<std::string, std::string>> config_file = read(argv[1]);

	if (config_file.size() == 0) {
		std::cerr << "Invalid config file.\n";
		return -1;
	}

	std::vector<key_value> values = parse(config_file);

	if (values.size() == 0) {
		return -1;
	}

	for (auto& i : values) {
		if (i.key == "updatesPerSecond")
			required_config[0] = true;
		if (i.key == "simulationTime")
			required_config[1] = true;
		if (i.key == "templateFile")
			required_config[2] = true;
	}

	if (std::find(required_config.begin(), required_config.end(), false) != required_config.end()) {
		std::cerr << "You need updatesPerSecond, simulationTime and tempalteFile in the config.\n";
		return -1;
	}

	int currentStep = 0;
	int ups = std::get<int>(find_element(values, "updatesPerSecond").value);
	int maxStep = ups * std::get<int>(find_element(values, "simulationTime").value);
	double timeStep = 1.0 / (double)ups;
	std::vector<Particle*> particles;

	//Testing

	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			Particle* new_particle = new Particle;
			Vec2D pos = Vec2D(x * 7.5 + 300, y * 7.5 + 200);
			new_particle->position = pos;
			if (x % 2 == 0)
				new_particle->position += Vec2D(0, 3.75);
			new_particle->id = y * 10 + x;
			new_particle->radius = 1;
			particles.push_back(new_particle);
		}
	}

	Particle* bigOne = new Particle;
	bigOne->position = Vec2D(100, 215);
	bigOne->mass = 400;
	bigOne->radius = 15;
	bigOne->velocity = Vec2D(250, 0);
	bigOne->affectedByDrag = false;
	bigOne->affectedBySprings = false;
	bigOne->id = 10000;
	particles.push_back(bigOne);

	//Not testing anymore

	int simulationUpdates = 4;
	int maxSimulationSteps = 16;
	double elapsedTime = timeStep / (double)simulationUpdates;

	std::ofstream output_file("output", std::ios::binary);
	output_file.write((char*)&maxStep, sizeof(int));
	int particleCount = particles.size();
	output_file.write((char*)&particleCount, sizeof(int));
	output_file.write((char*)&ups, sizeof(int));

	for (auto i : particles) {
		output_file.write((char*)&i->radius, sizeof(double));
	}

	auto startTime = std::chrono::system_clock::now();

	for (currentStep = 0; currentStep < maxStep; currentStep++) {
		QuadTree qt({Vec2D(0, 0), Vec2D(1280, 720)});
	
		for (auto& i : particles) {
			qt.insert(i);
		}

		//applyGravity(particles, 3);
		applyInverseGravity(particles, qt, 100);

		for (int k = 0; k < simulationUpdates; k++) {
			for (auto i : particles)
				i->simTimeRemaining = elapsedTime;

			applySpringForce(particles, qt, 0.3, 0.3, 8, 9, elapsedTime);

			for (int j = 0; j < maxSimulationSteps; j++) {
				for (int i = 0; i < particles.size(); i++) {			
					particles[i]->updatePhysics(elapsedTime);
				}

				resolveCollisions(particles, qt);
			}
		}

		for (auto i : particles) {
			output_file.write((char*)&i->position.x, sizeof(double));
			output_file.write((char*)&i->position.y, sizeof(double));
		}

		/*if (currentStep % 100 == 0) {
			auto endTime = std::chrono::system_clock::now();
			std::chrono::duration<double, std::milli> duration = endTime - startTime;
			std::cout << "Time for 100 steps: " << duration.count() << "ms\n";
			startTime = endTime;
		}*/

		std::cout << (currentStep+1) << '/' << maxStep << '\r'; 
	}

	std::cout << '\n';

	output_file.close();

	return 0;
}