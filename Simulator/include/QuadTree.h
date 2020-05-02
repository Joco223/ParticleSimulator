#pragma once

#include <vector>
#include <iostream>

#include "Vec2D.h"
#include "Particle.h"

struct Rect {
	Vec2D center;
	Vec2D size;

	bool contains(const Vec2D& other) {
		if (other.x > center.x-size.x && other.x < center.x+size.x) {
			if (other.y > center.y-size.y && other.y < center.y+size.y) {
				return true;
			}
		}
		return false;
	}

	bool intersects(const Rect& other) {
		return !(other.center.x - other.size.x > center.x + size.x ||
						 other.center.x + other.size.x < center.x - size.x ||
						 other.center.y - other.size.y > center.y + size.y ||
						 other.center.y + other.size.y < center.y - size.y);
	}
};

class QuadTree {
private:
	const int capacity = 4;
	bool subdivided = false;
	Rect boundary;
	std::vector<Particle*> particles;

	QuadTree* NW;
	QuadTree* NE;
	QuadTree* SW;
	QuadTree* SE;

	void queryRange(Rect& range, std::vector<Particle*>& tempParticles);

public:
	QuadTree(Rect boundary_) : boundary(boundary_) {particles.reserve(capacity);}

	bool insert(Particle* other);
	void subdivide();
	std::vector<Particle*> query(Rect& range);
};