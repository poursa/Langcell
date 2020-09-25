#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


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
		layout.Push<float>(2);
		layout.Push<float>(2);
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
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetCursorPosCallback(window, cursor_position_callback);
		glfwSetScrollCallback(window, scroll_callback);

		int speed = 50;
		float mutation = 0.0f;
		int conserve = 0;
		int red = 0, green = 0, blue = 0;
		int cell_norm_position_x = 0, cell_norm_position_y = 0;
		float zoomspeed = 1.3f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			ImGui_ImplGlfwGL3_NewFrame();


			shader.Bind();
			renderer.Draw(va, ib, shader);

			texture.Refresh(speed,mutation,conserve);
			shader.SetUniform1i("u_Texture", 0);


			cell_norm_position_x = g_xpos / (G_WIDTH / texture.GetWidth());
			cell_norm_position_y = g_ypos / (G_HEIGHT / texture.GetHeight());
			red = texture.getCell(cell_norm_position_x, cell_norm_position_y)->getColor().red;
			green = texture.getCell(cell_norm_position_x, cell_norm_position_y)->getColor().green;
			blue = texture.getCell(cell_norm_position_x, cell_norm_position_y)->getColor().blue;

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
					/*Make sure the new orthographic projection has proper values to a certain accuracy*/
					
					left = ((g_xpos * 2) / G_WIDTH - 1) + (left - ((g_xpos * 2) / G_WIDTH - 1)) / zoomspeed;
					right = ((g_xpos * 2) / G_WIDTH - 1) + (right - ((g_xpos * 2)/G_WIDTH - 1)  ) / zoomspeed;
					top = (1 - (g_ypos * 2) / G_HEIGHT ) + (top - (1 - (g_ypos * 2) / G_HEIGHT)) / zoomspeed;
					bottom = (1 - (g_ypos * 2) / G_HEIGHT) + (bottom - (1 - ((g_ypos * 2) / G_HEIGHT)) ) / zoomspeed;
				}
				else {
					left = -1, right = 1, top = 1, bottom = -1;
				}
				std::cout << left << " " << right << " " << top << " " << bottom << std::endl;
				proj = glm::ortho(left, right, bottom, top);
				mvp = proj * view * model;
				shader.SetUniformMat4f("u_MVP", mvp);
				g_yscroll = 0;
			}
			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double l_xpos, l_ypos;
		glfwGetCursorPos(window, &l_xpos, &l_ypos);
		g_xpos = (int)l_xpos;
		g_ypos = (int)l_ypos;
	}
}
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	double l_xpos, l_ypos;
	glfwGetCursorPos(window, &l_xpos, &l_ypos);
	g_xpos = (int)l_xpos;
	g_ypos = (int)l_ypos;
	g_yscroll = yoffset;
}
