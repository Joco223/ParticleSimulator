#pragma once

#include "Vec2D.h"

#include <vector>
#include <iostream>
#include <cmath>

class Particle {
private:
	bool detectCollision(const Particle& other);
	void handleCollision(Particle& other);

	Vec2D lerp(const Vec2D& p1, const Vec2D& p2, double t);
public:
	Vec2D position, velocity, acceleration;
	double mass, radius;
	int type = 0; //Will be used later on for visualizer

	Particle();
	
	void applyCollision(std::vector<Particle>& others, int index);
	void applyGravity(const double grav_const, const std::vector<Particle>& others);
	void updatePhysics(double timeStep);
};