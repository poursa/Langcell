#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>

#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui//imgui_impl_glfw_gl3.h"

#define G_WIDTH  1000.f
#define G_HEIGHT 1000.f

int g_yscroll = 0;
int g_xpos = 0, g_ypos = 0;
float x_offset = 0, y_offset = 0;
bool offset = false;

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(G_WIDTH, G_HEIGHT, "Langcell", NULL, NULL);
	glfwSetWindowSizeLimits(window, GLFW_DONT_CARE, GLFW_DONT_CARE, G_WIDTH, G_HEIGHT);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/*Frame rate restriction*/
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cout << "Error!" << std::endl;
	}
	
	std::cout << glGetString(GL_VERSION) << std::endl;



	
	{
		float positions[] = {
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f ,1.0f
		};

		unsigned int indices[] = {
			0,1,2,
			2,3,0
		};
		
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));
		

		/*A vertex array is bound along with a buffer filled with data at the appropriate size
		A layout is chosen for that vertex buffer and it is added to the vertex array
		An index buffer is chosen 
		A shader is chosen,bound and has a uniform added to it*/
		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);//Vertex coordinates
		layout.Push<float>(2);//Texture coordinates
		va.AddBuffer(vb, layout);
		IndexBuffer ib(indices, 6);

		float left = -1.0f, right = 1.0f, top = 1.0f, bottom = -1.0f;
		glm::mat4 proj = glm::ortho(left, right, bottom, top, -1.0f,1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f),glm::vec3(0,0,0));
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		glm::mat4 mvp = proj * view * model;
		
		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_MVP", mvp);

		Texture texture("res/textures/europe.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);
		/*Everything is Unbound*/
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
		/*Renderer initialized*/
		Renderer renderer;

		/*Init UI*/
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();
		GLCall(glfwSetMouseButtonCallback(window, mouse_button_callback));
		GLCall(glfwSetCursorPosCallback(window, cursor_position_callback));
		GLCall(glfwSetScrollCallback(window, scroll_callback));
		GLCall(glfwSetKeyCallback(window, key_callback));

		int speed = 50;
		float mutation = 0.0f;
		int conserve = 0;
		int red = 0, green = 0, blue = 0;
		int cell_norm_position_x = 0, cell_norm_position_y = 0;
		float zoomspeed = 1.3f;
		float ortho_mouse_pos_x = 0, ortho_mouse_pos_y = 0;
		float centerx = 0, centery = 0;
		float new_left, new_right, new_bottom, new_top;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			ImGui_ImplGlfwGL3_NewFrame();


			shader.Bind();
			renderer.Draw(va, ib, shader);

			texture.Refresh(speed, mutation, conserve);
			shader.SetUniform1i("u_Texture", 0);

			if (g_xpos >= 0 && g_xpos <= G_WIDTH && g_ypos >= 0 && g_ypos <= G_HEIGHT) {
				cell_norm_position_x = g_xpos / (G_WIDTH / texture.GetWidth());
				cell_norm_position_y = g_ypos / (G_HEIGHT / texture.GetHeight());
				red = texture.getCell(cell_norm_position_x, cell_norm_position_y)->getColor().red;
				green = texture.getCell(cell_norm_position_x, cell_norm_position_y)->getColor().green;
				blue = texture.getCell(cell_norm_position_x, cell_norm_position_y)->getColor().blue;
			}
			{
				ImGui::SliderInt("Speed", &speed, 0, 1000);
				ImGui::SliderInt("Conservation", &conserve, 0, 1000);
				ImGui::SliderFloat("Mutation", &mutation, 0.0f, 10.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::Text("R: %d G: %d B: %d", red , green, blue);
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			/*Zoom*/
			if (g_yscroll != 0) {
				if (g_yscroll > 0) {
					new_left = left / zoomspeed;
					new_right = right / zoomspeed;
					new_top = top / zoomspeed;
					new_bottom = bottom / zoomspeed;
				}
				else {
					new_left = left * zoomspeed;
					new_right = right * zoomspeed;
					new_top = top * zoomspeed;
					new_bottom = bottom * zoomspeed;
				}
				if ((new_left + 0.01 < new_right && new_bottom + 0.01 < new_top)
					&& (new_left >= -1 && new_right <= 1 && new_bottom >= -1 && new_top <= 1)) {
					left = new_left;
					right = new_right;
					bottom = new_bottom;
					top = new_top;
				}
				std::cout << left << " " << right << " " << top << " " << bottom << std::endl;
				proj = glm::ortho(left, right, bottom, top);
				mvp = proj * view * model;
				shader.SetUniformMat4f("u_MVP", mvp);
				g_yscroll = 0;
			}
			else {
				new_left = left;
				new_right = right;
				new_top = top;
				new_bottom = bottom;
			}
			if (offset) {
				new_left += x_offset;
				new_right += x_offset;
				new_top += y_offset;
				new_bottom += y_offset;
				if (new_left >= -1 && new_right <= 1 && new_bottom >= -1 && new_top <= 1) {
					left = new_left;
					right = new_right;
					bottom = new_bottom;
					top = new_top;
				}
				std::cout << left << " " << right << " " << top << " " << bottom << std::endl;
				proj = glm::ortho(left, right, bottom, top);
				mvp = proj * view * model;
				shader.SetUniformMat4f("u_MVP", mvp);
				x_offset = 0;
				y_offset = 0;
				offset = false;
			}
			/* Swap front and back buffers */
			GLCall(glfwSwapBuffers(window));

			/* Poll for and process events */
			GLCall(glfwPollEvents());
		}
		ImGui_ImplGlfwGL3_Shutdown();
		ImGui::DestroyContext();
	}
	glfwTerminate();
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double l_xpos, l_ypos;
		GLCall(glfwGetCursorPos(window, &l_xpos, &l_ypos));
		g_xpos = (int)l_xpos;
		g_ypos = (int)l_ypos;
	}
}
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	g_yscroll = yoffset;
}
