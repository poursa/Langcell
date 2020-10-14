#pragma once

#include "Renderer.h"


class Callbacks {
public:
	
	Callbacks(GLFWwindow* window);

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static int g_yscroll;
	static int g_xpos, g_ypos;
	static float x_offset, y_offset;
	static bool offset;
};