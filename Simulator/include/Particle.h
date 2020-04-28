#pragma once

#include "Vec2D.h"

#include <vector>
#include <iostream>

class Particle {
public:
	Vec2D position, velocity, acceleration;
	double mass, radius;
	int type = 0; //Will be used later on for visualizer

	Particle();
	
	void applyGravity(const double grav_const, const std::vector<Particle>& others);
	void updatePhysics(double timeStep);
};