
#include "Callbacks.h"




double Callbacks::g_yscroll;
int Callbacks::g_xpos, Callbacks::g_ypos;
float Callbacks::x_offset, Callbacks::y_offset;
bool Callbacks::offset;

Callbacks::Callbacks(GLFWwindow* window)
{
	g_yscroll = 0;
	g_xpos = 0, g_ypos = 0 ;
	x_offset = 0, y_offset = 0;
	offset = false;
	GLCall(glfwSetMouseButtonCallback(window, mouse_button_callback));
	GLCall(glfwSetCursorPosCallback(window, cursor_position_callback));
	GLCall(glfwSetScrollCallback(window, scroll_callback));
	GLCall(glfwSetKeyCallback(window, key_callback));
}

void Callbacks::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double l_xpos, l_ypos;
		GLCall(glfwGetCursorPos(window, &l_xpos, &l_ypos));
		g_xpos = (int)l_xpos;
		g_ypos = (int)l_ypos;
	}
}
void Callbacks::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
}
void Callbacks::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		y_offset += 0.1;
		offset = true;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		y_offset += -0.1;
		offset = true;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		x_offset += -0.1;
		offset = true;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		x_offset += 0.1;
		offset = true;
	}
}
void Callbacks::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	g_yscroll = yoffset;

}
