#pragma once

#include "Vec2D.h"

#include <vector>
#include <iostream>
#include <cmath>

struct line {
	Vec2D start, end;
	double thickness;
};

class Particle {
public:
	Vec2D position, oldPosition, velocity, acceleration;
	double mass, radius, simTimeRemaining, heat;
	double meltingPoint, boilingPoint, viscosity;
	int type = 0; //Will be used later on for visualizer
	int id;
	bool affectedByGravity = true;
	bool affectedBySprings = true;
	bool affectedByDrag = true;

	Particle();
	Particle(Vec2D position_, int id_) : position(position_), id(id_) {}
	
	bool detectCollision(const Particle* other);
	void updatePhysics(double timeStep);
};