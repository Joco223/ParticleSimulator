#pragma once

#include "Particle.h"
#include "Vec2D.h"
#include "QuadTree.h"

#include <iostream>
#include <algorithm>
#include <vector>

void resolveCollisions(std::vector<Particle*>& particles, QuadTree& qt) {
	std::vector<std::pair<Particle*, Particle*>> collidingPairs;

	//Resolve static collision
	for (auto i : particles) {
		Rect target = {i->position, Vec2D(i->radius*20 + 0.5, i->radius*20 + 0.5)};
		std::vector<Particle*> neighbours = qt.query(target);
		for (auto j : neighbours) {
			if (i->id != j->id) {
				if (i->detectCollision(j)) {
					collidingPairs.push_back({i, j});

					double dist = i->position.dist(j->position);

					double overlap = 0.5 * (dist - i->radius - j->radius);

					double offsetX = overlap * (i->position.x - j->position.x) / dist;
					double offsetY = overlap * (i->position.y - j->position.y) / dist;

					i->position.x -= offsetX;
					i->position.y -= offsetY;

					j->position.x += offsetX;
					j->position.y += offsetY;
				}
			}
		}
	}

	//Resolve dynamic collision
	for (auto i : collidingPairs) {
		Particle* b1 = i.first;
		Particle* b2 = i.second;

		double dist = b1->position.dist(b2->position);

		double nx = (b2->position.x - b1->position.x) / dist;
		double ny = (b2->position.y - b1->position.y) / dist;

		double tx = -ny;
		double ty = nx;

		double dpTan1 = b1->velocity.x * tx + b1->velocity.y * ty;
		double dpTan2 = b2->velocity.x * tx + b2->velocity.y * ty;

		double dpNorm1 = b1->velocity.x * nx + b1->velocity.y * ny;
		double dpNorm2 = b2->velocity.x * nx + b2->velocity.y * ny;

		double m1 = ((dpNorm2 - dpNorm1)*b2->mass*0.8 + dpNorm1*b1->mass + dpNorm2*b2->mass) / (b1->mass + b2->mass);
		double m2 = ((dpNorm1 - dpNorm2)*b1->mass*0.8 + dpNorm1*b1->mass + dpNorm2*b2->mass) / (b1->mass + b2->mass);

		b1->velocity.x = tx * dpTan1 + nx * m1;
		b1->velocity.y = ty * dpTan1 + ny * m1;
		b2->velocity.x = tx * dpTan2 + nx * m2;
		b2->velocity.y = ty * dpTan2 + ny * m2;
	}
}

void applyGravity(std::vector<Particle*>& particles, double gravConst) {
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i]->affectedByGravity) {
			for (int j = i + 1; j < particles.size(); j++) {
				if (particles[j]->affectedByGravity) {
					Particle* p1 = particles[i];
					Particle* p2 = particles[j];
					
					double distance = p1->position.rawDist(p2->position);
					double force = (gravConst * p1->mass * p2->mass) * (1.0 / distance);
					Vec2D accForce = ((p1->position - p2->position).fastNorm() * -force);
					p1->acceleration += accForce;
					p2->acceleration -= accForce;
				}		
			}
		}
	}
}