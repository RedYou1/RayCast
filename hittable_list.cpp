#include "hittable_list.h"

hittable_list::~hittable_list()
{
	for (hittable* object : objects)
		delete object;
}

hittable* hittable_list::hit(const ray& r, float& t_min, float& t_max, hit_record& rec) const {
	hittable* hit_anything = nullptr;

	for (hittable* object : objects) {
		if (object->isHit(r, t_min, t_max)) {
			hit_anything = object;
		}
	}

	return hit_anything;
}
