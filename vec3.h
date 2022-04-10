#pragma once
#include <cmath>
#include <iostream>
#include "utilities.h"

using std::sqrt;

class vec3 {
public:
	inline static vec3 random() {
		return vec3(random_float(), random_float(), random_float());
	}

	inline static vec3 random(float min, float max) {
		return vec3(random_float(min, max), random_float(min, max), random_float(min, max));
	}

	bool near_zero() const {
		// Return true if the vector is close to zero in all dimensions.
		const auto s = 1e-8;
		return (fabs(x) < s) && (fabs(y) < s) && (fabs(z) < s);
	}

	vec3() : x(0), y(0), z(0) {}
	vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

	float X() const { return x; }
	float Y() const { return y; }
	float Z() const { return z; }

	vec3 operator-() const { return vec3(-x, -y, -z); }

	vec3& operator+=(const vec3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3& operator*=(const float t) {
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	vec3& operator/=(const float t) {
		return *this *= 1 / t;
	}

	float length() const {
		return sqrt(length_squared());
	}

	float length_squared() const {
		return x * x + y * y + z * z;
	}

public:
	float x;
	float y;
	float z;
};

// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.x << ' ' << v.y << ' ' << v.z;
}

inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline vec3 operator*(float t, const vec3& v) {
	return vec3(t * v.x, t * v.y, t * v.z);
}

inline vec3 operator*(const vec3& v, float t) {
	return t * v;
}

inline vec3 operator/(vec3 v, float t) {
	return (1 / t) * v;
}

inline float dot(const vec3& u, const vec3& v) {
	return u.x * v.x
		+ u.y * v.y
		+ u.z * v.z;
}

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x);
}

inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}

inline vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}

inline vec3 random_in_unit_sphere() {
	return vec3::random(-1, 1) / 3;
}

inline vec3 random_unit_vector() {
	return unit_vector(random_in_unit_sphere());
}
inline vec3 random_in_hemisphere(const vec3& normal) {
	vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}

inline vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat) {
	float cos_theta = (float)fmin(dot(-uv, n), 1.0);
	vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	vec3 r_out_parallel = ((float)-sqrt(fabs(1.0 - r_out_perp.length_squared()))) * n;
	return r_out_perp + r_out_parallel;
}

inline vec3 random_in_unit_disk() {
	return vec3(random_float(-1, 1), random_float(-1, 1), 0) / 2;
}