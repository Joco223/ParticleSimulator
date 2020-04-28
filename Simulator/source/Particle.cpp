#include "Particle.h"

Particle::Particle() {
	position = Vec2D();
	velocity = Vec2D();
	acceleration = Vec2D();
	mass = 20;
	radius = 1;
}

void Particle::applyGravity(const double grav_const, const std::vector<Particle>& others) {
	Vec2D finalForce = Vec2D();

	for (auto& i : others) {
		double distance = position.dist(i.position);
		if (distance < 0.0001)
			continue;
		double force = (grav_const * mass * i.mass) / (distance*distance);
		Vec2D forceDir = (position - i.position).norm() * -force;
		finalForce += forceDir;
	}

	acceleration = (finalForce / mass);
}

void Particle::updatePhysics(double timeStep) {
	velocity += (acceleration / mass) * timeStep;
	position += velocity * timeStep;
	acceleration = Vec2D();
}