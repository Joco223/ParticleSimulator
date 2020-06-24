#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <math.h>

#include "Vec2D.h"

std::vector<std::vector<Vec2D>> loadFile(const std::string input, int& ups, int& maxSteps, int& particleCount, std::vector<double>& radii, std::vector<sf::ConvexShape>& lines) {
	std::vector<std::vector<Vec2D>> particles;
	
	std::ifstream input_file(input, std::ios::in);
	if (!input_file.is_open()) {
		std::cerr << "Cannot open input file,\n";
		return particles;
	}

	input_file.read((char*)&maxSteps, sizeof(int));
	input_file.read((char*)&particleCount, sizeof(int));
	input_file.read((char*)&ups, sizeof(int));

	for (int i = 0; i < particleCount; i++) {
		double r;
		input_file.read((char*)&r, sizeof(double));
		radii.push_back(r);
	}

	int lineCount;
	input_file.read((char*)&lineCount, sizeof(int));

	for (int i = 0; i < lineCount; i++) {
		double x, y, thickness;
		input_file.read((char*)&x, sizeof(double));
		input_file.read((char*)&y, sizeof(double));
		sf::Vector2f start(x - 1, y - 1);

		input_file.read((char*)&x, sizeof(double));
		input_file.read((char*)&y, sizeof(double));
		sf::Vector2f end(x - 1, y - 1);

		input_file.read((char*)&thickness, sizeof(double));

		sf::ConvexShape newLine;

		double angle = Vec2D::bearing(Vec2D(start.x, start.y), Vec2D(end.x, end.y));
		double currentAngle = angle + M_PI*2;
		newLine.setPointCount(20);

		for (int j = 0; j < 10; j++) {
			double newX = thickness * std::cos(currentAngle) + start.x;
			double newY = thickness * std::sin(currentAngle) + start.y;
			newLine.setPoint(j, sf::Vector2f(newX, newY));

			currentAngle += M_PI/10;
		}

		currentAngle = angle + M_PI*2;

		for (int j = 0; j < 10; j++) {
			double newX = thickness * std::cos(currentAngle) + end.x;
			double newY = thickness * std::sin(currentAngle) + end.y;
			newLine.setPoint(19-j, sf::Vector2f(newX, newY));

			currentAngle -= M_PI/10;
		}

		newLine.setFillColor(sf::Color(200, 200, 200));
		lines.push_back(newLine);
	}

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

	return particles;
}

Vec2D getParticlePos(float currentStep, int index, const std::vector<std::vector<Vec2D>>& particles) {
	int wholeStep = std::floor(currentStep);
	float partialStep = currentStep - wholeStep;
	double newXPos, newYPos;

	if (partialStep < 0.000001) {
		newXPos = particles[wholeStep][index].x;
		newYPos = particles[wholeStep][index].y;
	}else{
		newXPos = std::lerp(particles[wholeStep][index].x, particles[wholeStep+1][index].x, (double)partialStep);
		newYPos = std::lerp(particles[wholeStep][index].y, particles[wholeStep+1][index].y, (double)partialStep);
	}

	return Vec2D(newXPos, newYPos);
}