#pragma once
#include "hittable.h"

#include <memory>
#include <vector>

class hittable_list {
public:
	std::vector<hittable*> objects;
public:
	hittable_list() :objects{} {}
	hittable_list(hittable* object) { add(object); }
	~hittable_list();
	void add(hittable* object) { objects.push_back(object); }

	hittable* hit(
		const ray& r, float& t_min, float& t_max, hit_record& rec) const;
};