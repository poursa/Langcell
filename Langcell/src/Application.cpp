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

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1000, 1000, "Langcell", NULL, NULL);
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

		glm::mat4 proj = glm::ortho(-1.0f,1.0f,-1.0f, 1.0f, -1.0f,1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f),glm::vec3(0,0,0));
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));


		glm::mat4 mvp = proj * view * model;

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_MVP", mvp);

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


		int speed = 50;
		float mutation = 0.0f;
		float closeness = 0.5f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			ImGui_ImplGlfwGL3_NewFrame();


			shader.Bind();
			//shader.SetUniform4f("u_Color", red, 0.3f, 0.8f, 1.0f);

			renderer.Draw(va, ib, shader);

			texture.Refresh(speed,mutation,closeness);
			shader.SetUniform1i("u_Texture", 0);

			{                       
				ImGui::SliderInt("Speed", &speed, 0, 100);
				ImGui::SliderFloat("Mutation", &mutation, 0.0f, 10.0f);
				ImGui::SliderFloat("Closeness", &closeness, 0.0f, 1.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

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