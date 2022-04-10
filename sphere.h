#pragma once
#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
	sphere(point3 cen, float r, shared_ptr<material> m)
		: center(cen), radius(r), radius2(r* r), mat_ptr(m) {};

	virtual bool hit(
		const ray& r, float t_min, float t_max, hit_record& rec) const override;

public:
	point3 center;
	float radius;
	float radius2;
	shared_ptr<material> mat_ptr;
};