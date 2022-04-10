#include "Renderer.h"
#include "ray.h"
#include "camera.h"

#if !profiler
//use only the main thread when profiling
const int Renderer::max_thread = std::thread::hardware_concurrency() - 3;
#endif

float clamp(float x, float min, float max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

Renderer::Renderer(std::string filename, camera* camera, hittable_list* world, int width, int height)
	:out(filename), m_lines(), m_nextExport(0),
	m_nextLine(0), m_width(width), m_height(height),
#if !profiler
	m_threads(new std::thread[max_thread]),
#endif
	m_lock(), m_camera(camera), m_world(world)
{
	if (!out.is_open())
		throw "";

	out << "P3\n" << width << " " << height << "\n255\n";

#if !profiler
	for (int i(0); i < max_thread; i++)
		m_threads[i] = std::thread(renderThread, this);
#endif
	renderThread(this);
#if !profiler
	for (int i(0); i < max_thread; i++)
		m_threads[i].join();
#endif
}

Renderer::~Renderer()
{
	out.close();
	for (std::map<int, unsigned char*>::iterator it(m_lines.begin()); it != m_lines.end(); ++it)
		delete[] it->second;
	m_lines.clear();
#if !profiler
	delete[] m_threads;
#endif
	while (!m_unused.empty()) {
		delete[] m_unused.front();
		m_unused.pop_front();
	}
}

void Renderer::renderThread(Renderer* renderer) {
	unsigned char* l{ nullptr };
	int y{ 0 };
	while (true) {
		renderer->getNextLine(y, l);
		if (y >= renderer->m_height) {
			delete[] l;
			return;
		}

		for (int x{ 0 }; x < image_width; ++x) {
#if samples
			color pixel_color(0, 0, 0);
			for (float sx(-1.0f); sx < 1.0f; sx += samples_diff) {
				for (float sy(-1.0f); sy < 1.0f; sy += samples_diff) {
					ray r = renderer->m_camera->get_ray(
						(x + sx) / (image_width - 1),
						((image_height - y - 1) + sy) / (image_height - 1));
					pixel_color += renderer->ray_color(r, max_depth);
				}
			}
			int i{ x * 3 };
			// Divide the color by the number of samples and gamma-correct for gamma=2.0.
			l[i] = static_cast<unsigned char>(256 * clamp(sqrt(samples_scale * pixel_color.X()), 0, 0.999f));
			l[i + 1] = static_cast<unsigned char>(256 * clamp(sqrt(samples_scale * pixel_color.Y()), 0, 0.999f));
			l[i + 2] = static_cast<unsigned char>(256 * clamp(sqrt(samples_scale * pixel_color.Z()), 0, 0.999f));
#else
			float u = ((float)x) / (image_width - 1);
			float v = ((float)image_height - y - 1) / (image_height - 1);
			ray r = renderer->m_camera->get_ray(u, v);
			color pixel_color(renderer->ray_color(r, max_depth));
			int i{ x * 3 };
			l[i] = static_cast<unsigned char>(256 * clamp(sqrt(pixel_color.X()), 0, 0.999f));
			l[i + 1] = static_cast<unsigned char>(256 * clamp(sqrt(pixel_color.Y()), 0, 0.999f));
			l[i + 2] = static_cast<unsigned char>(256 * clamp(sqrt(pixel_color.Z()), 0, 0.999f));
#endif
		}
	}
}

color Renderer::ray_color(const ray& r, int depth) {
	hit_record rec;

	if (depth <= 0)
		return color(0, 0, 0);

	float min(0.001f);
	float max(infinity);
	if (hittable* hit = m_world->hit(r, min, max, rec)) {
		hit->hit(r, max, rec);

		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, depth - 1);
		return color(0, 0, 0);
	}

	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5f * (unit_direction.Y() + 1.0f);
	return (1.0f - t) * color(1.0f, 1.0f, 1.0f) + t * color(0.5f, 0.7f, 1.0f);
}

void Renderer::addLine(int y, unsigned char*& line)
{
	m_lines.insert(std::pair<int, unsigned char*>(y, line));
}

void Renderer::exportImg() {
	while (m_lines.find(m_nextExport) != m_lines.end()) {
		unsigned char* l{ m_lines[m_nextExport] };
		int size{ m_width * 3 };
		int i(0);
		while (i < size) {
			out << static_cast<unsigned>(l[i++]) << ' ';
			out << static_cast<unsigned>(l[i++]) << ' ';
			out << static_cast<unsigned>(l[i++]) << '\n';
		}
		m_unused.push_front(l);
		m_lines.erase(m_nextExport);
		m_nextExport++;
	}
}

unsigned char* Renderer::getLine()
{
	unsigned char* l{ nullptr };
	if (!m_unused.empty()) {
		l = m_unused.front();
		m_unused.pop_front();
	}
	else {
		l = new unsigned char[m_width * 3];
	}
	return l;
}

void Renderer::getNextLine(int& pre, unsigned char*& line)
{
	std::lock_guard<std::mutex> lg(m_lock);

	if (line != nullptr) {
		std::cout << pre << "/" << m_height << std::endl;
		addLine(pre, line);
	}
	line = getLine();
	pre = m_nextLine;
	m_nextLine++;
	exportImg();
}
