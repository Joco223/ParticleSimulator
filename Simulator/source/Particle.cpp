#include "Particle.h"

Particle::Particle() {
	position = Vec2D();
	velocity = Vec2D();
	acceleration = Vec2D();
	mass = 5;
	radius = 1;
}

Vec2D Particle::lerp(const Vec2D& p1, const Vec2D& p2, double t) {
	Vec2D newVec;
	newVec.x = std::lerp(p1.x, p2.x, t);
	newVec.y = std::lerp(p1.y, p2.y, t);
	return newVec;
}

bool Particle::detectCollision(const Particle* other) {
		double distance = position.rawDist(other->position);

		if (distance < (radius+other->radius)*(radius+other->radius)) {
			return true;
		}else{
			return false;
		}
}

void Particle::handleCollision(Particle* other) {
	Vec2D delta = position - other->position;
	double d = delta.len();
	Vec2D mtd = delta * ((radius + other->radius - d)/d);

	double im1 = 1.0 / mass; 
	double im2 = 1.0 / other->mass;

	position += mtd * (im1 / (im1 + im2));
	other->position -= mtd * (im2 / (im1 + im2));

	Vec2D v = velocity - other->velocity;
	double vn = Vec2D::dot(v, mtd.norm());

	if (vn > 0.0) return;

	double i = (-(1.0 + 0.8) * vn) / (im1 / im2);
	Vec2D impulse = mtd.norm() * i;

	velocity += impulse * im1;
	other->velocity -= impulse * im2;
}

void Particle::applyGravity(const double grav_const, const std::vector<Particle*>& others, int index) {
	for (int i = index + 1; i < others.size(); i++) {
		double distance = position.rawDist(others[i]->position);
		double force = (grav_const * mass * others[i]->mass) * (1.0 / distance);
		Vec2D accForce = ((position - others[i]->position).fastNorm() * -force);
		acceleration += accForce;
		others[i]->acceleration -= accForce;
	}
}

void Particle::applyCollision(std::vector<Particle*>& others) {
	for (int i = 0; i < others.size(); i++) {
		if (id != others[i]->id) {
			if (detectCollision(others[i])) {
				handleCollision(others[i]);
			}
		}
	}
}

void Particle::updatePhysics(double timeStep) {
	velocity += (acceleration * (1.0/mass)) * timeStep;
	position += velocity * timeStep;
	acceleration = Vec2D();
}