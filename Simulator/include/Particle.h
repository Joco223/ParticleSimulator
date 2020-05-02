#pragma once

#include "Vec2D.h"

#include <vector>
#include <iostream>
#include <cmath>

class Particle {
private:
	bool detectCollision(const Particle* other);
	void handleCollision(Particle* other);

	Vec2D lerp(const Vec2D& p1, const Vec2D& p2, double t);
public:
	Vec2D position, velocity, acceleration;
	double mass, radius;
	int type = 0; //Will be used later on for visualizer
	int id;

	Particle();
	Particle(Vec2D position_, int id_) : position(position_), id(id_) {}
	
	void applyCollision(std::vector<Particle*>& others);
	void applyGravity(const double grav_const, const std::vector<Particle*>& others, int index);
	void updatePhysics(double timeStep);
};