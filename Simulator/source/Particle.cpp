#include "Particle.h"

Particle::Particle() {
	position = Vec2D();
	velocity = Vec2D();
	acceleration = Vec2D();
	mass = 20;
	radius = 1;
}

Vec2D Particle::lerp(const Vec2D& p1, const Vec2D& p2, double t) {
	Vec2D newVec;
	newVec.x = std::lerp(p1.x, p2.x, t);
	newVec.y = std::lerp(p1.y, p2.y, t);
	return newVec;
}

bool Particle::detectCollision(const Particle& other) {
		double diffX = position.x - other.position.x;
		double diffY = position.y - other.position.y;

		if (diffX*diffX + diffY*diffY < (radius+other.radius)*(radius+other.radius)) {
			return true;
		}else{
			return false;
		}
}

void Particle::handleCollision(Particle& other) {
	Vec2D delta = position - other.position;
	double d = delta.len();
	Vec2D mtd = delta * ((radius + other.radius - d)/d);

	double im1 = 1.0 / mass; 
	double im2 = 1.0 / other.mass;

	position += mtd * (im1 / (im1 + im2));
	other.position -= mtd * (im2 / (im1 + im2));

	Vec2D v = velocity - other.velocity;
	double vn = Vec2D::dot(v, mtd.norm());

	if (vn > 0.0) return;

	double i = (-(1.0 + 0.8) * vn) / (im1 / im2);
	Vec2D impulse = mtd.norm() * i;

	velocity += impulse * im1;
	other.velocity -= impulse * im2;
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

void Particle::applyCollision(std::vector<Particle>& others, int index) {
	for (int i = index + 1; i < others.size(); i++) {
		if (detectCollision(others[i])) {
			handleCollision(others[i]);
		}
	}
}

void Particle::updatePhysics(double timeStep) {
	velocity += (acceleration / mass) * timeStep;
	position += velocity * timeStep;
	acceleration = Vec2D();
}