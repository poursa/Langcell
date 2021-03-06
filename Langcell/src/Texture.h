#pragma once

#include <future>

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
	std::vector<std::future<void>> m_Futures;
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	void UpdateCell(int i, int j, float mutation, int conserve);
	void Refresh(unsigned int speed, float mutation, int conserve);
	void SaveCell(int i, int j);

	Cell* getCell(int x, int y);
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	
};