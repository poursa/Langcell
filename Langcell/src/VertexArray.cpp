#include "VertexArray.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"

VertexArray::VertexArray()
{
	/*Generate a unique vertex array ID*/
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
	/*Delete the current vertex array*/
	GLCall(glDeleteVertexArrays(1,&m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer & vb, const VertexBufferLayout & layout)
{
	/*Bind the VertexArray and continue by binding the vertex buffer and setting it's layout so that they are linked*/
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{
	/*Bind the current Vertex Array*/
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	/*Bind a 0 Vertex Array, essentially unbinding the current one*/
	GLCall(glBindVertexArray(0));
}
