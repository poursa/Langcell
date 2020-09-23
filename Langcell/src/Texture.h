#pragma once

#include "Renderer.h"
#include "Cell.h"

class Texture {
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	std::vector<std::vector<Cell*>> cells;
	unsigned int m_update;
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	void Refresh(unsigned int speed, float mutation, float closeness);

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

};