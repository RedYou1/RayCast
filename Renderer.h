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
#include <forward_list>

class Renderer {
private:
#if !profiler
	static const int max_thread;
#endif

	std::ofstream out;
	std::map<int, unsigned char*> m_lines;
	std::forward_list<unsigned char*> m_unused;
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
	void getNextLine(int& pre, unsigned char*& line);
	unsigned char* getLine();
	inline static void renderThread(Renderer* renderer);
	color ray_color(const ray& r, int depth);
	void addLine(int y, unsigned char*& line);
	void exportImg();
};