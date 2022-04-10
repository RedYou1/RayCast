#include "sphere.h"

sphere::~sphere()
{
	delete mat_ptr;
}

bool sphere::isHit(const ray& r, float& t_min, float& t_max) {
	const vec3 oc(r.origin() - center);
	const vec3 dir(r.direction());
	const float a(dir.length_squared());
	const float half_b(dot(oc, dir));

	const float discriminant(half_b * half_b - a * (oc.length_squared() - radius2));
	if (discriminant < 0) return false;
	const float sqrtd(sqrt(discriminant));

	// Find the nearest root that lies in the acceptable range.
	float root((-half_b - sqrtd) / a);
	if (root < t_min || t_max < root) {
		root = (-half_b + sqrtd) / a;
		if (root < t_min || t_max < root)
			return false;
	}
	t_max = root;
	return true;
}

void sphere::hit(const ray& r, float& root, hit_record& rec) {
	rec.t = root;
	rec.p = r.at(root);
	vec3 outward_normal((rec.p - center) / radius);
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mat_ptr;
}
