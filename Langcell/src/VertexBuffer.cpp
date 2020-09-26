#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer(const void * data, unsigned int size)
{
	/*Generate a unique ID for the buffer*/
	GLCall(glGenBuffers(1, &m_RendererID));
	/*Using that ID generate the buffer*/
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	/*Pass the data into the buffer*/
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
}

VertexBuffer::VertexBuffer()
{
	/*Delete the buffer using its stored ID*/
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
	/*Bind the current Buffer*/
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const
{
	/*Bind Buffer 0, essentially unbinding the old one*/
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
