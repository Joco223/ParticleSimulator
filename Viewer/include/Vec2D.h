#pragma once

#include <cmath>

class Vec2D {
public:
	double x, y;

	Vec2D() : x(0), y(0) {}
	Vec2D(double _x, double _y) : x(_x), y(_y) {}
	Vec2D(const Vec2D& other) : x(other.x), y(other.y) {}

	Vec2D& operator=(const Vec2D& other) {
		x = other.x;
		y = other.y;
		return *this;
	}

	Vec2D operator+(const Vec2D& other) {
		return Vec2D(x + other.x, y + other.y);
	}

	Vec2D operator-(const Vec2D& other) {
		return Vec2D(x - other.x, y - other.y);
	}

	Vec2D& operator+=(const Vec2D& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	Vec2D& operator-=(const Vec2D& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vec2D operator+(const double other) {
		return Vec2D(x + other, y + other);
	}

	Vec2D operator-(const double other) {
		return Vec2D(x - other, y - other);
	}

	Vec2D& operator+=(const double other) {
		x += other;
		y += other;
		return *this;
	}

	Vec2D& operator-=(const double other) {
		x -= other;
		y -= other;
		return *this;
	}

	Vec2D operator*(const double scale) {
		return Vec2D(x*scale, y*scale);
	}

	Vec2D& operator*=(const double other) {
		x *= other;
		y *= other;
		return *this;
	}

	Vec2D operator/(const double scale) {
		return Vec2D(x/scale, y/scale);
	}

	Vec2D& operator/=(const double other) {
		x /= other;
		y /= other;
		return *this;
	}

	Vec2D& operator/(const Vec2D& other) {
		x /= other.x;
		y /= other.y;
		return *this;
	}

	double len() const {
		return std::sqrt(x*x + y*y);
	}

	Vec2D& norm() {
		if (len() < 0.00001) return *this;
		*this *= (1.0 / len());
		return *this;
	}

	double dist(const Vec2D& other) const {
		return Vec2D(x - other.x, y - other.y).len();
	}

	static double dot(const Vec2D& v1, const Vec2D& v2) {
		return v1.x * v2.x + v1.y + v2.y;
	}

	static double cross(const Vec2D& v1, const Vec2D& v2) {
		return (v1.x * v2.y) - (v1.y * v2.x);
	}

	static double bearing(const Vec2D& v1, const Vec2D& v2) {
    double TWOPI = 6.2831853071795865;
    double theta = std::atan2(v2.x - v1.x, v1.y - v2.y);
    if (theta < 0.0)
        theta += TWOPI;
    return theta;
	}
};