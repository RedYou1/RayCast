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

hittable_list* random_scene();

int main()
{
	{
		srand((unsigned int)time(NULL));

		hittable_list* world{ random_scene() };

		point3 lookfrom(13, 2, 3);
		point3 lookat(0, 0, 0);
		vec3 vup(0, 1, 0);
		float dist_to_focus = 10.0f;
		float aperture = 0.1f;

		camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

		std::cout << "Begin" << std::endl;

#if samples
		Renderer renderer{ "test.ppm",&cam,world,image_width,image_height };
#else
		Renderer renderer{ "test.ppm",&cam,world,image_width,image_height };
#endif

		std::cout << "output image..." << std::endl;

		renderer.exportImg();

		delete world;

		std::cout << "End" << std::endl;
	}
}

hittable_list* random_scene() {
	hittable_list* world = new hittable_list();

	lambertian* ground_material = new lambertian(color(0.5f, 0.5f, 0.5f));
	world->add(new sphere(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = random_float();
			point3 center(a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float());

			if ((center - point3(4, 0.2f, 0)).length() > 0.9f) {
				material* sphere_material;

				if (choose_mat < 0.6) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = new lambertian(albedo);
					world->add(new sphere(center, 0.2f, sphere_material));
				}
				else {
					// metal
					auto albedo = color::random(0.5f, 1);
					auto fuzz = random_float(0, 0.5f);
					sphere_material = new metal(albedo, fuzz);
					world->add(new sphere(center, 0.2f, sphere_material));
				}
			}
		}
	}

	metal* material1 = new metal(color(0.2f, 0.7f, 0.1f), 0);
	world->add(new sphere(point3(0, 1, 0), 1.0f, material1));

	lambertian* material2 = new lambertian(color(0.4f, 0.2f, 0.1f));
	world->add(new sphere(point3(-4, 1, 0), 1.0f, material2));

	metal* material3 = new metal(color(0.7f, 0.6f, 0.5f), 0);
	world->add(new sphere(point3(4, 1, 0), 1.0f, material3));

	return world;
}