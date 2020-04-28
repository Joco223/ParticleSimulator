#include "CfgParser.h"
#include "Particle.h"

#include <iostream>
#include <unordered_map>
#include <variant>
#include <algorithm>
#include <fstream>

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
	std::vector<Particle> particles;

	//Testing

	for (int y = 0; y < 20; y++) {
		for (int x = 0; x < 20; x++) {
			Particle new_particle;
			new_particle.position = Vec2D(x * 15 + 200, y * 15 + 200);
			particles.push_back(new_particle);
		}
	}

	//Not testing anymore

	std::ofstream output_file("output", std::ios::binary);
	output_file.write((char*)&maxStep, sizeof(int));
	int particleCount = particles.size();
	output_file.write((char*)&particleCount, sizeof(int));
	output_file.write((char*)&ups, sizeof(int));

	for (currentStep = 0; currentStep < maxStep; currentStep++) {
		for (auto& i : particles) {
			i.applyGravity(20, particles);
			i.updatePhysics(timeStep);
		}

		for (auto& i : particles) {
			output_file.write((char*)&i.position.x, sizeof(double));
			output_file.write((char*)&i.position.y, sizeof(double));
		}

		std::cout << (currentStep+1) << '/' << maxStep << '\r'; 
	}

	std::cout << '\n';

	output_file.close();

	return 0;
}