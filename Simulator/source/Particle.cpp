#include "Particle.h"

Particle::Particle() {
	position = Vec2D();
	velocity = Vec2D();
	acceleration = Vec2D();
	mass = 2;
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
	if (simTimeRemaining > 0.0) {
		oldPosition = position;

		/*if (affectedByDrag) {
			acceleration.x += -velocity.x * 0.99;
			acceleration.y += -velocity.y * 0.99;
		}*/

		velocity += acceleration * simTimeRemaining;
		if (affectedByDrag) 
			velocity = velocity * (1 - simTimeRemaining * 0.3);

		if (velocity.rawLen() < 0.05) {
			velocity = Vec2D();
		}

		position += velocity * simTimeRemaining;
		acceleration = Vec2D();
	}
}