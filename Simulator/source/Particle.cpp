#include "Particle.h"

Particle::Particle() {
	position = Vec2D();
	velocity = Vec2D();
	acceleration = Vec2D();
	mass = 5;
	radius = 1;
}

bool Particle::detectCollision(const Particle* other) {
		double distance = position.rawDist(other->position);

		if (distance < (radius+other->radius)*(radius+other->radius)) {
			return true;
		}else{
			return false;
		}
}

void Particle::updatePhysics(double timeStep) {
	velocity += (acceleration * (1.0/mass)) * timeStep;
	position += velocity * timeStep;
	acceleration = Vec2D();
}