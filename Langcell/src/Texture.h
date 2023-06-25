#pragma once

#include <future>

#include "Renderer.h"
#include "Cell.h"
#define TEX_THREADS 8

class Texture {
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	std::vector<std::vector<Cell>> cells;
	unsigned int m_update;
	std::vector<std::future<void>> m_Futures;
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	void Refresh(unsigned int speed, float mutation, int conserve_mut, int conserve_inf, bool colored);

	void UpdateCell(int i, int j, float mutation, int conserve_mut, int conserve_inf);


	void SaveCell(int i, int j);

	Cell* getCell(int x, int y);
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	
};