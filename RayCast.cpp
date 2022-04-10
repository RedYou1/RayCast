#include <iostream>
#include "Renderer.h"
#include "ray.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "utilities.h"
#include <thread>
#include <string>
#include <chrono>
#include <queue>

//https://raytracing.github.io/books/RayTracingInOneWeekend.html

hittable_list random_scene();

static auto world = random_scene();

static point3 lookfrom(13, 2, 3);
static point3 lookat(0, 0, 0);
static vec3 vup(0, 1, 0);
static float dist_to_focus = 10.0f;
static float aperture = 0.1f;

static camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

int main()
{
	srand((unsigned int)time(NULL));
	std::cout << "Begin" << std::endl;

#if samples
	Renderer renderer{ "test.ppm",&cam,&world,image_width,image_height };
#else
	Renderer renderer{ "test.ppm",&cam,&world,image_width,image_height };
#endif

	std::cout << "output image..." << std::endl;

	renderer.exportImg();

	std::cout << "End" << std::endl;
}

hittable_list random_scene() {
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.5f, 0.5f, 0.5f));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = random_float();
			point3 center(a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float());

			if ((center - point3(4, 0.2f, 0)).length() > 0.9f) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.6) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2f, sphere_material));
				}
				else {
					// metal
					auto albedo = color::random(0.5f, 1);
					auto fuzz = random_float(0, 0.5f);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2f, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<metal>(color(0.2f, 0.7f, 0.1f), 0);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0f, material1));

	auto material2 = make_shared<lambertian>(color(0.4f, 0.2f, 0.1f));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0f, material2));

	auto material3 = make_shared<metal>(color(0.7f, 0.6f, 0.5f), 0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0f, material3));

	return world;
}