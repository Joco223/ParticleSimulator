#include "CfgParser.h"
#include "Particle.h"
#include "QuadTree.h"
#include "Vec2D.h"

#include <iostream>
#include <unordered_map>
#include <variant>
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

	for (int y = 0; y < 40; y++) {
		for (int x = 0; x < 40; x++) {
			Particle* new_particle = new Particle;
			Vec2D pos = Vec2D(x * 7.5 + 200, y * 7.5 + 200);
			new_particle->position = pos;
			new_particle->id = y * 40 + x;
			new_particle->radius = 1;
			particles.push_back(new_particle);
		}
	}	

	//Not testing anymore

	std::ofstream output_file("output", std::ios::binary);
	output_file.write((char*)&maxStep, sizeof(int));
	int particleCount = particles.size();
	output_file.write((char*)&particleCount, sizeof(int));
	output_file.write((char*)&ups, sizeof(int));

	auto startTime = std::chrono::system_clock::now();

	for (currentStep = 0; currentStep < maxStep; currentStep++) {
		QuadTree qt({Vec2D(0, 0), Vec2D(1280, 720)});
	
		for (auto& i : particles) {
			qt.insert(i);
		}
		
		for (int i = 0; i < particles.size(); i++) {
			Rect target = {particles[i]->position, Vec2D(particles[i]->radius*3, particles[i]->radius*3)};
			std::vector<Particle*> neighbours = qt.query(target);

			particles[i]->applyGravity(10, particles, i);			
			particles[i]->applyCollision(neighbours);
			particles[i]->updatePhysics(timeStep);			
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