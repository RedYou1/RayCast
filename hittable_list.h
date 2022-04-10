#pragma once
#include "hittable.h"

#include <memory>
#include <vector>

class hittable_list {
public:
	hittable_list() {}
	hittable_list(hittable* object) { add(object); }
	~hittable_list();
	void add(hittable* object) { objects.push_back(object); }

	hittable* hit(
		const ray& r, float& t_min, float& t_max, hit_record& rec) const;

public:
	std::vector<hittable*> objects;
};