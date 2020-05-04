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
	false, //TF
	false, //Simulation splits
	false  //MI
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
		if (i.key == "simulationSplits")
			required_config[3] = true;
		if (i.key == "maxIterations")
			required_config[4] = true;
	}

	if (std::find(required_config.begin(), required_config.end(), false) != required_config.end()) {
		std::cerr << "You need updatesPerSecond, simulationTime, tempalteFile, simulationSplits and maxIterations in the config.\n";
		return -1;
	}

	int currentStep = 0;
	int ups = std::get<int>(find_element(values, "updatesPerSecond").value);
	int maxStep = ups * std::get<int>(find_element(values, "simulationTime").value);
	double timeStep = 1.0 / (double)ups;
	std::vector<Particle*> particles;
	std::vector<line> lines;
	lines.push_back({Vec2D(0, 720), Vec2D(1280, 720), 3});
	lines.push_back({Vec2D(0, 0), Vec2D(0, 720), 3});
	lines.push_back({Vec2D(1280, 0), Vec2D(1280, 720), 3});

	//Testing

	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			Particle* new_particle = new Particle;
			Vec2D pos = Vec2D(x * 3 + 200, y * 3 + 600);
			new_particle->position = pos;
			if (x % 2 == 0)
				new_particle->position += Vec2D(0, 1.5);
			new_particle->id = y * 10 + x;
			new_particle->radius = 1;
			new_particle->affectedByDrag = true;
			particles.push_back(new_particle);
		}
	}

	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			Particle* new_particle = new Particle;
			Vec2D pos = Vec2D(x * 3 + 1000, y * 3 + 585);
			new_particle->position = pos;
			if (x % 2 == 0)
				new_particle->position += Vec2D(0, 1.5);
			new_particle->id = y * 10 + x + 10000;
			new_particle->radius = 1;
			new_particle->type = 2;
			new_particle->affectedByDrag = true;
			particles.push_back(new_particle);
		}
	}

	/*Particle* bigOne = new Particle;
	bigOne->position = Vec2D(100, 215);
	bigOne->mass = 400;
	bigOne->radius = 15;
	bigOne->velocity = Vec2D(250, 0);
	bigOne->affectedByDrag = false;
	bigOne->affectedBySprings = false;
	bigOne->id = 10000;
	particles.push_back(bigOne);*/

	//Not testing anymore

	int simulationUpdates = std::get<int>(find_element(values, "simulationSplits").value);
	int maxSimulationSteps = std::get<int>(find_element(values, "maxIterations").value);
	double elapsedTime = timeStep / (double)simulationUpdates;

	std::ofstream output_file("output", std::ios::binary);
	output_file.write((char*)&maxStep, sizeof(int));
	int particleCount = particles.size();
	output_file.write((char*)&particleCount, sizeof(int));
	output_file.write((char*)&ups, sizeof(int));

	for (auto i : particles) {
		output_file.write((char*)&i->radius, sizeof(double));
	}

	int lineCount = lines.size();
	output_file.write((char*)&lineCount, sizeof(int));

	for (auto& i : lines) {
		output_file.write((char*)&i.start.x, sizeof(double));
		output_file.write((char*)&i.start.y, sizeof(double));
		output_file.write((char*)&i.end.x, sizeof(double));
		output_file.write((char*)&i.end.y, sizeof(double));
		output_file.write((char*)&i.thickness, sizeof(double));
	}

	auto startTime = std::chrono::system_clock::now();

	for (currentStep = 0; currentStep < maxStep; currentStep++) {
		QuadTree qt({Vec2D(0, 0), Vec2D(1280, 720)});
	
		for (auto& i : particles) {
			qt.insert(i);
		}

		//applyGravity(particles, 3);
		//applyInverseGravity(particles, qt, 100);

		if (currentStep == 1200) {
			for (int i = 0; i < 100; i++) {
				particles[i]->velocity = Vec2D(200, 0);
			}
			for (int i = 100; i < 200; i++) {
				particles[i]->velocity = Vec2D(-200, 0);
			}
		}

		for (int k = 0; k < simulationUpdates; k++) {
			for (auto i : particles)
				i->simTimeRemaining = elapsedTime;

			
			//applyGlobalGravity(particles, 100);
			

			for (int j = 0; j < maxSimulationSteps; j++) {		
				applySpringForce(particles, qt, 5, 15, 3, 4, elapsedTime);

				for (int i = 0; i < particles.size(); i++) {			
					particles[i]->updatePhysics(elapsedTime);
				}

				resolveCollisions(particles, qt, lines);
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