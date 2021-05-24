// engine.cpp

#include <sys/time.h>	// gettimeofday

#include "engine.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "renderer.hpp"

#define MAX_DT 1.0f

Engine engine;

static void engine_initialize(Engine* engine);
static i32 engine_run(Engine* engine);

void engine_initialize(Engine* engine) {
	engine->is_running = 1;
	engine->animation_playing = 0;
	engine->delta_time = 0;
	engine->total_time = 0;
	engine->mouse_x = 0;
	engine->mouse_y = 0;
	camera_initialize(V3(13, 3, 9));
}

i32 engine_run(Engine* engine) {
	float angle = 0.0f;
    float shine = 1.0f;
	struct timeval now = {0};
	struct timeval prev = {0};
	while (engine->is_running && window_poll_events() >= 0) {
		prev = now;
		gettimeofday(&now, NULL);
		engine->delta_time = ((((now.tv_sec - prev.tv_sec) * 1000000.0f) + now.tv_usec) - (prev.tv_usec)) / 1000000.0f;
		if (engine->delta_time >= MAX_DT) {
			engine->delta_time = MAX_DT;
		}
		if (engine->animation_playing) {
			engine->total_time += engine->delta_time;
		}
		if (key_down[GLFW_KEY_SPACE]) {
			engine->animation_playing = !engine->animation_playing;
		}
		if (key_down[GLFW_KEY_ESCAPE]) {
			engine->is_running = 0;
		}
		if (key_down[GLFW_KEY_F11]) {
			window_toggle_fullscreen();
		}
        if (key_down[GLFW_KEY_UP]) {
            shine += 0.1;
            printf("Shine: %f\n", shine);
        }
        if (key_down[GLFW_KEY_DOWN]) {
            shine -= 0.1;
            printf("Shine: %f\n", shine);
        }

		window_get_cursor(&engine->mouse_x, &engine->mouse_y);
		camera_update();

		//render_mesh(V3(0, 0, -8), V3(0, angle, 0), V3(1, 1, 1), 1, 0.0f);	// TODO(lucas): Temporary, remove and replace with a decently proper scene managemenet/animation system
		//render_mesh(V3(0, 0, 0), V3(0, angle, 0), V3(3, 3, 3), 2, 1.0f);

        render_mesh(V3(0, 0, -8), V3(0, angle, 0), V3(1, 1, 1), (Material){
          .emission = 0.0f,
          .shininess = shine,
          .texture_id = 1
        });

        render_mesh(V3(0, 0, 0), V3(0, angle, 0), V3(3, 3, 3), (Material){
          .emission = 1.0f,
          .shininess = 80.0f,
          .texture_id = 2
        });

		angle += 5 * engine->delta_time;

		window_swap_buffers();
		window_clear_buffers(0, 0, 0);
	}
	return NoError;
}

i32 engine_start() {
	i32 result = NoError;
	engine_initialize(&engine);

	if ((result = window_open("Solar System", 800, 600, 0 /* fullscreen */, 0 /* vsync */)) == NoError) {
		renderer_initialize();
		engine_run(&engine);
		window_close();
		renderer_destroy();
	}
	assert("memory leak" && (memory_total_allocated() == 0));
	return result;
}
