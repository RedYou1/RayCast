#pragma once

#define samples true
#define profiler false

static const double aspect_ratio = 3.0 / 2.0;//3.0/2.0
#if profiler
static const int image_width = 50;
#else
static const int image_width = 1200;//1200
#endif
static const int image_height = static_cast<int>(image_width / aspect_ratio);
#if samples
static const int samples_per_pixel = 75;//500
static const float samples_diff = 2.0f / (float)sqrt(samples_per_pixel);
static const float samples_scale = 1.0f / samples_per_pixel;
#endif
static const int max_depth = 50;//50


const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385f;

inline float random_float() {
	// Returns a random real in [0,1).
	return (float)(rand() / (RAND_MAX + 1.0));
}

inline float random_float(float min, float max) {
	// Returns a random real in [min,max).
	return (float)(min + (max - min) * random_float());
}

inline float degrees_to_radians(float degrees) {
	return (float)(degrees * pi / 180.0);
}