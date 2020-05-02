#pragma once

#include "Vec2D.h"

#include <vector>
#include <iostream>
#include <cmath>

class Particle {
public:
	Vec2D position, velocity, acceleration;
	double mass, radius;
	int type = 0; //Will be used later on for visualizer
	int id;

	Particle();
	Particle(Vec2D position_, int id_) : position(position_), id(id_) {}
	
	bool detectCollision(const Particle* other);
	void updatePhysics(double timeStep);
};