#include "material.h"
#include "hittable.h"
#include "vec3.h"
#include "utilities.h"

bool lambertian::scatter(
	const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
) {
#if samples
	vec3 scatter_direction(rec.normal + random_unit_vector());
#else
	vec3 scatter_direction(rec.normal);
#endif
	scattered = ray(rec.p, scatter_direction);
	attenuation = albedo;
	return true;
}

bool metal::scatter(
	const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
) {
	vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
#if samples
	scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
#else
	scattered = ray(rec.p, reflected);
#endif
	attenuation = albedo;
	return (dot(scattered.direction(), rec.normal) > 0);
}
