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

	//bool isDrawable(int pos);
	void Refresh(unsigned int speed, float mutation, int conserve);

	Cell* getCell(int x, int y);
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	
};