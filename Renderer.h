#pragma once
#include <iostream>
#include <fstream>
#include "vec3.h"
#include <map>
#include <queue>
#include <thread>
#include "camera.h"
#include "hittable_list.h"
#include <mutex>

class Line {
private:
	unsigned char* m_pixels;
public:
	Line(int width);
	~Line();
	void setPixel(int x, vec3 color);
	unsigned char getPixel(int x);
};

class Renderer {
private:
#if !profiler
	static const int max_thread;
#endif

	std::ofstream out;
	std::map<int, Line*> m_lines;
	std::thread* m_threads;

	camera* m_camera;
	hittable_list* m_world;

	std::mutex m_lock;
	int m_nextExport;
	int m_nextLine;
	int m_width;
	int m_height;
public:
	Renderer(std::string filename, camera* camera, hittable_list* world, int width, int height);
	~Renderer();
	int getNextLine();
	inline static void renderThread(Renderer* renderer);
	color ray_color(const ray& r, int depth);
	void addLine(int y, Line* line);
	void exportImg();
};