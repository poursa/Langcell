#include <iostream>

#include <algorithm>
#include <thread>

#include "stb_image/stb_image.h"

#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Callbacks.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui//imgui_impl_glfw_gl3.h"

#define G_WIDTH  1000
#define G_HEIGHT 1000

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
		glm::mat4 view = glm::translate(glm::mat4(1.0f),glm::vec3(0, 0, 0));
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		glm::mat4 mvp = proj * view * model;
		
		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_MVP", mvp);

		GLFWimage images[1];
		images[0].pixels = stbi_load("res/textures/icon.png", &images[0].width, &images[0].height, 0, STBI_rgb_alpha);
		glfwSetWindowIcon(window, 1, images);
		stbi_image_free(images[0].pixels);

		Texture texture("res/textures/worldmapnew.png");
		
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
		/*Settings Callbacks*/
		Callbacks cbs(window);

		//Sliders and buttons
		int speed = 50;
		float mutation_rate = 0.0f, influence_rate = 0.0f;
		float conserve_mut = 0, conserve_infl = 0;
		unsigned int colored = 0;

		//Text output
		int red = 0, green = 0, blue = 0;

		//Zooming and panning 
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

			texture.Refresh(speed, mutation_rate, influence_rate, conserve_mut / 100.0f, conserve_infl / 100.0f, colored);
			shader.SetUniform1i("u_Texture", 0);
			
			if (cbs.g_xpos >= 0 && cbs.g_xpos <= G_WIDTH && cbs.g_ypos >= 0 && cbs.g_ypos <= G_HEIGHT) {
				cell_norm_position_x = cbs.g_xpos / (G_WIDTH / texture.GetWidth());
				cell_norm_position_y = cbs.g_ypos / (G_HEIGHT / texture.GetHeight());
				red = texture.getCell(cell_norm_position_x, cell_norm_position_y)->getColor(colored).red;
				green = texture.getCell(cell_norm_position_x, cell_norm_position_y)->getColor(colored).green;
				blue = texture.getCell(cell_norm_position_x, cell_norm_position_y)->getColor(colored).blue;
			}
			{
				ImGui::SliderInt("Speed", &speed, 0, 1000);
				ImGui::SliderFloat("Mutation conservation", &conserve_mut, 0, 100, "%.2f %%");
				ImGui::SliderFloat("Influence conservation", &conserve_infl, 0, 100, "%.2f %%");
				ImGui::SliderFloat("Mutation", &mutation_rate, 0.0f, 50.0f);
				ImGui::SliderFloat("Influence", &influence_rate, 0.0f, 10.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::Text("R: %d G: %d B: %d x: %d y: %d", red, green, blue, cell_norm_position_x, cell_norm_position_y);
				if (ImGui::SmallButton("Color")) colored = (colored + 1) % 4;
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());


			/*Zoom*/
			if (cbs.g_yscroll != 0) {

				if (cbs.g_yscroll > 0) {
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
				//zoom out immediately if you're at the edge of the screen
				else if (new_left >= -1 || new_right <= 1 || new_bottom >= -1 || new_top <= 1) {
					left = -1.0f;
					right = 1.0f;
					top = 1.0f;
					bottom = -1.0f;
				}
				
				std::cout << left << " " << right << " " << top << " " << bottom << std::endl;
				proj = glm::ortho(left, right, bottom, top);
				mvp = proj * view * model;
				shader.SetUniformMat4f("u_MVP", mvp);
				cbs.g_yscroll = 0;
			}
			//else {
			//	new_left = left;
			//	new_right = right;
			//	new_top = top;
			//	new_bottom = bottom;
			//}
			if (cbs.offset) {
				new_left += cbs.x_offset;
				new_right += cbs.x_offset;
				new_top += cbs.y_offset;
				new_bottom += cbs.y_offset;
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
				cbs.x_offset = 0;
				cbs.y_offset = 0;
				cbs.offset = false;
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
void zoom() {

}