#pragma once

#include "Particle.h"
#include "Vec2D.h"
#include "QuadTree.h"

#include <iostream>
#include <algorithm>
#include <vector>

void resolveCollisions(std::vector<Particle*>& particles, QuadTree& qt, std::vector<line>& lines) {
	std::vector<std::pair<Particle*, Particle*>> collidingPairs;
	std::vector<Particle*> fakeParticles;

	//Resolve static collision
	for (auto i : particles) {
		for (auto& j : lines) {
			double lineX1 = j.end.x - j.start.x;
			double lineY1 = j.end.y - j.start.y;

			double lineX2 = i->position.x - j.start.x;
			double lineY2 = i->position.y - j.start.y;

			double edgeLength = lineX1*lineX1 + lineY1*lineY1;

			double t = std::max(0.0, std::min(edgeLength, (lineX1*lineX2 + lineY1*lineY2))) / edgeLength;
		
			Vec2D closestPoint(j.start.x + t * lineX1, j.start.y + t * lineY1);

			double dist = closestPoint.dist(i->position);

			if (dist < i->radius + j.thickness) {
				Particle* fakeParticle = new Particle;
				fakeParticle->radius = j.thickness;
				fakeParticle->mass = i->mass;
				fakeParticle->position = closestPoint;
				fakeParticle->velocity = i->velocity * -1;

				fakeParticles.push_back(fakeParticle);
				collidingPairs.push_back({i, fakeParticle});

				double overlap = dist - i->radius - fakeParticle->radius;

				i->position.x -= overlap * (i->position.x - fakeParticle->position.x) / dist;
				i->position.y -= overlap * (i->position.y - fakeParticle->position.y) / dist;
			}
		}
	}


	for (auto i : particles) {
		Rect target = {i->position, Vec2D(i->radius + 21, i->radius + 21)};
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

		if (i->simTimeRemaining > 0.0) {
			double intentedSpeed = i->velocity.len();
			double intentedDist = intentedSpeed * i->simTimeRemaining;
			double actualDist = (i->position - i->oldPosition).len();
			double actualTime = actualDist / intentedSpeed;
			i->simTimeRemaining -= actualTime;
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

	for (auto i : fakeParticles)
		delete i;
	fakeParticles.clear();
	collidingPairs.clear();
}

double cosineLerp(double p1, double p2, double m) {
	double m2 = (1.0 - cos(m * 3.141593)) / 2.0;
	return(p1 * (1.0 - m2) + p2 * m2);
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

void applyInverseGravity(std::vector<Particle*>& particles, QuadTree& qt, double invGravConst) {
	for (auto i : particles) {
		Rect target = {i->position, Vec2D(i->radius + 20, i->radius + 20)};
		std::vector<Particle*> neighbours = qt.query(target);
		for (auto j : neighbours) {
			if (i->id != j->id) {
				double distance = 1.0 / (i->position.dist(j->position) / ((i->heat - i->boilingPoint) / (i->viscosity*75.0)));
				double force = std::max(0.0, invGravConst * distance);
				Vec2D accForce = ((i->position - j->position).fastNorm() * force);
				i->acceleration += accForce;
			}
		}
	}
}

void applySpringForce(std::vector<Particle*>& particles, QuadTree& qt, double springConstant, double dampingConstant, double equiDistance, double maxDistance, double timeStep) {
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i]->affectedBySprings) {
			for (int j = i + 1; j < particles.size(); j++) {
				if (particles[j]->affectedBySprings && particles[i]->type == particles[j]->type) {
					double dist = particles[i]->position.dist(particles[j]->position);
					if (dist < maxDistance) {
						Vec2D springDir = (particles[j]->position - particles[i]->position).fastNorm();
						double goalDist = 0.5*dist - 0.5*equiDistance;
						double goalSpeed = springConstant * (goalDist / timeStep);
						double dampingSpeed = dampingConstant * (equiDistance / dist);

						double m1 = std::min(1.0, (particles[i]->heat / particles[i]->meltingPoint));
						double springStengthI = cosineLerp(1.0, 0.0, m1)*particles[i]->viscosity;
						double m2 = std::min(1.0, (particles[j]->heat / particles[j]->meltingPoint));
						double springStengthJ = cosineLerp(1.0, 0.0, m2)*particles[j]->viscosity;

						particles[j]->velocity -= springDir * ((goalSpeed - dampingSpeed) * timeStep) * springStengthI;
						particles[i]->velocity += springDir * ((goalSpeed - dampingSpeed) * timeStep) * springStengthJ;
					}
				}
			}
		}
	}
}

void applyGlobalGravity(std::vector<Particle*>& particles, double gravConstant, bool up = false) {
	for (auto i : particles) {
		if (!up) {
			i->acceleration.y += gravConstant * i->mass;
		}else{
			i->acceleration.y -= gravConstant * i->mass;
		}		
	}
}