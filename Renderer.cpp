#include "Renderer.h"
#include "ray.h"
#include "camera.h"

#if !profiler
//use only the main thread when profiling
const int Renderer::max_thread = std::thread::hardware_concurrency() - 3;
#endif

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
	m_lines.clear();
#if !profiler
	delete[] m_threads;
#endif
}

void Renderer::renderThread(Renderer* renderer) {
	Line* l{ nullptr };
	int y{ 0 };
	while (true) {
		y = (renderer->getNextLine(y, l));
		if (y >= renderer->m_height)
			return;

		l = new Line{ image_width };

		for (int i = 0; i < image_width; ++i) {
			color pixel_color(0, 0, 0);
#if samples
			for (float sx(-1.0f); sx < 1.0f; sx += samples_diff) {
				for (float sy(-1.0f); sy < 1.0f; sy += samples_diff) {
					ray r = renderer->m_camera->get_ray(
						(i + sx) / (image_width - 1),
						((image_height - y - 1) + sy) / (image_height - 1));
					pixel_color += renderer->ray_color(r, max_depth);
				}
			}
			l->setPixel(i, pixel_color);
#else
			float u = ((float)i) / (image_width - 1);
			float v = ((float)image_height - y - 1) / (image_height - 1);
			ray r = renderer->m_camera->get_ray(u, v);
			pixel_color += renderer->ray_color(r, max_depth);
			l->setPixel(i, pixel_color);
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

void Renderer::addLine(int y, Line* line)
{
	m_lines.insert(std::pair<int, Line*>(y, line));
}

float clamp(float x, float min, float max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

void Renderer::exportImg() {
	while (m_lines.find(m_nextExport) != m_lines.end()) {
		Line l{ *m_lines[m_nextExport] };
		int size{ m_width * 3 };
		for (int i(0); i < size; i += 3) {
			out << static_cast<unsigned>(l.getPixel(i)) << ' '
				<< static_cast<unsigned>(l.getPixel(i + 1)) << ' '
				<< static_cast<unsigned>(l.getPixel(i + 2)) << '\n';
		}
		m_lines.erase(m_nextExport);
		m_nextExport++;
	}
}

int Renderer::getNextLine(int pre, Line* line)
{
	while (!m_lock.try_lock()) {}
	if (line != nullptr)
		addLine(pre, line);
	int y(m_nextLine);
	m_nextLine++;
	std::cout << m_nextLine << "/" << m_height << std::endl;
	exportImg();
	m_lock.unlock();
	return y;
}

Line::Line(int width)
	:m_pixels(new unsigned char[width * 3])
{
}

Line::~Line()
{
	delete[] m_pixels;
}

void Line::setPixel(int x, vec3 color)
{
#if samples
	// Divide the color by the number of samples and gamma-correct for gamma=2.0.
	float scale = 1.0f / samples_per_pixel;
	m_pixels[x * 3] = static_cast<unsigned>(256 * clamp(sqrt(scale * color.X()), 0, 0.999f));
	m_pixels[x * 3 + 1] = static_cast<unsigned>(256 * clamp(sqrt(scale * color.Y()), 0, 0.999f));
	m_pixels[x * 3 + 2] = static_cast<unsigned>(256 * clamp(sqrt(scale * color.Z()), 0, 0.999f));
#else
	m_pixels[x * 3] = static_cast<unsigned>(256 * clamp(sqrt(color.X()), 0, 0.999f));
	m_pixels[x * 3 + 1] = static_cast<unsigned>(256 * clamp(sqrt(color.Y()), 0, 0.999f));
	m_pixels[x * 3 + 2] = static_cast<unsigned>(256 * clamp(sqrt(color.Z()), 0, 0.999f));
#endif
}

unsigned char Line::getPixel(int x)
{
	return m_pixels[x];
}
