#pragma once
#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
	sphere(point3 cen, float r, material* m)
		: center(cen), radius(r), radius2(r* r), mat_ptr(m) {};
	~sphere() override { delete mat_ptr; }

	virtual bool isHit(
		const ray& r, float& t_min, float& t_max) const override;
	virtual void hit(
		const ray& r, float& root, hit_record& rec) const override;

public:
	point3 center;
	float radius;
	float radius2;
	material* mat_ptr;
};