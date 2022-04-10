#pragma once
#include "ray.h"
#include "material.h"

using std::shared_ptr;

struct hit_record {
	point3 p;
	vec3 normal;
	material* mat_ptr;
	float t;
	bool front_face;

	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable {
public:
	virtual ~hittable() = default;
	virtual bool isHit(const ray& r, float& t_min, float& t_max) = 0;
	virtual void hit(const ray& r, float& root, hit_record& rec) = 0;
};