// engine.hpp

#ifndef _ENGINE_HPP
#define _ENGINE_HPP

#include "common.hpp"
#include "matrix_math.hpp"

typedef struct Engine {
	u8 is_running;
	u8 animation_playing;
	float delta_time;
	float total_time;
	double mouse_x;
	double mouse_y;
} Engine;

extern Engine engine;

i32 engine_start();

#endif