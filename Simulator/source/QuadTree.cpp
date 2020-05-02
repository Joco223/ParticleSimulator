#include "QuadTree.h"

bool QuadTree::insert(Particle* other) {
	if (!boundary.contains(other->position)) return false;

	if (particles.size() < capacity && !subdivided) {
		particles.push_back(other);
		return true;
	}

	if (!subdivided) {
		subdivide();
		subdivided = true;
	}

	if (NW->insert(other)) return true;
	if (NE->insert(other)) return true;
	if (SW->insert(other)) return true;
	if (SE->insert(other)) return true;

	return false;
}

void QuadTree::queryRange(Rect& range, std::vector<Particle*>& tempParticles) {
	if (!boundary.intersects(range)) return;

	for (int i = 0; i < particles.size(); i++) {
		if (range.contains(particles[i]->position)) {
			tempParticles.push_back(particles[i]);
		}
	}

	if (!subdivided) return;

	NW->queryRange(range, tempParticles);
	NE->queryRange(range, tempParticles);
	SW->queryRange(range, tempParticles);
	SE->queryRange(range, tempParticles);
}

std::vector<Particle*> QuadTree::query(Rect& range) {
	std::vector<Particle*> finalParticles;
	queryRange(range, finalParticles);
	return finalParticles;
}

void QuadTree::subdivide() {
	NW = new QuadTree({Vec2D(boundary.center.x - boundary.size.x/2, boundary.center.y - boundary.size.y/2),
										 Vec2D(boundary.size.x/2,  boundary.size.y/2)});
	NE = new QuadTree({Vec2D(boundary.center.x + boundary.size.x/2, boundary.center.y - boundary.size.y/2),
										 Vec2D(boundary.size.x/2,  boundary.size.y/2)});
	SW = new QuadTree({Vec2D(boundary.center.x - boundary.size.x/2, boundary.center.y + boundary.size.y/2),
										 Vec2D(boundary.size.x/2,  boundary.size.y/2)});
	SE = new QuadTree({Vec2D(boundary.center.x + boundary.size.x/2, boundary.center.y + boundary.size.y/2),
										 Vec2D(boundary.size.x/2,  boundary.size.y/2)});
}