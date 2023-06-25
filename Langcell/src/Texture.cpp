#include "Texture.h"
#include <iostream>
#include "Cell.h"
#include <thread>

#include "stb_image/stb_image.h"




Texture::Texture(const std::string & path)
	:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0), m_update(0)
{
	stbi_set_flip_vertically_on_load(1);

	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	std::cout << m_Height << m_Width << std::endl;

	constexpr bool colored = true;
	cells.reserve(m_Height);

	for (size_t i = 0; i < m_Height; i++) {
		std::vector<Cell> row;
		row.reserve(m_Width);
		for (size_t j = 0; j < m_Width; j++) {
			size_t pos = i * m_Width * 4 + j*4;
			
			row.emplace_back();
			Cell& cell = row.back();
			if (m_LocalBuffer[pos] == 0 && m_LocalBuffer[pos + 1] == 0 && m_LocalBuffer[pos + 2] == 0 && m_LocalBuffer[pos + 3] == 255) {
				m_LocalBuffer[pos] = (unsigned char)cell.getColor(colored).red;
				m_LocalBuffer[pos + 1] = (unsigned char)cell.getColor(colored).green;
				m_LocalBuffer[pos + 2] = (unsigned char)cell.getColor(colored).blue;
			}
			else if(m_LocalBuffer[pos] == 1 && m_LocalBuffer[pos + 1] == 38 && m_LocalBuffer[pos + 2] == 255 && m_LocalBuffer[pos + 3] == 255){
				cell.setWater(true);
			}
			else if (m_LocalBuffer[pos + 3] == 100) {
				m_LocalBuffer[pos] = (unsigned char)cell.getColor(colored).red;
				m_LocalBuffer[pos + 1] = (unsigned char)cell.getColor(colored).green;
				m_LocalBuffer[pos + 2] = (unsigned char)cell.getColor(colored).blue;
				m_LocalBuffer[pos + 3] = 255;
				cell.setRiver(true);
			}
			else if(m_LocalBuffer[pos] == 150 && m_LocalBuffer[pos + 1] == 80 && m_LocalBuffer[pos + 2] == 0 && m_LocalBuffer[pos + 3] == 255) {
				m_LocalBuffer[pos] = (unsigned char)cell.getColor(colored).red;
				m_LocalBuffer[pos + 1] = (unsigned char)cell.getColor(colored).green;
				m_LocalBuffer[pos + 2] = (unsigned char)cell.getColor(colored).blue;
				m_LocalBuffer[pos + 3] = 255;
				cell.setMount(true);
			}
			else {
				cell.setWater(true);
			}
		}
		cells.push_back(row);
	}
	/*Set Neighbors*/
	for (size_t i = 0; i < m_Height; i++) {
		for (size_t j = 0; j < m_Width; j++) {
			//int pos = i * m_Height * 4 + j * 4;
			if (!cells.at(i).at(j).isWater()) {
				Cell* topn, *botn, *rightn, *leftn;
				if (i == 0) {
					topn = nullptr;
					botn = &cells.at(i + 1).at(j);
					if (botn->isWater()) {
						botn = nullptr;
					}
				}else if (i == m_Height - 1) {
					topn = &cells.at(i - 1).at(j);
					botn = nullptr;
					if (topn->isWater()) {
						topn = nullptr;
					}
				}else{
					topn = &cells.at(i - 1).at(j);
					botn = &cells.at(i + 1).at(j);
					if (topn->isWater()) {
						topn = nullptr;
					}
					if (botn->isWater()) {
						botn = nullptr;
					}
				}
				if (j == 0) {
					rightn = &cells.at(i).at(j + 1);
					leftn = nullptr;
					if (rightn->isWater()) {
						rightn = nullptr;
					}
				}else if (j == m_Width - 1 ){
					rightn = nullptr;
					leftn = &cells.at(i).at(j - 1);
					if (leftn->isWater()) {
						leftn = nullptr;
					}
				}else{
					rightn = &cells.at(i).at(j + 1);
					leftn = &cells.at(i).at(j - 1);
					if (rightn->isWater()) {
						rightn = nullptr;
					}
					if (leftn->isWater()) {
						leftn = nullptr;
					}
				}
				cells.at(i).at(j).setNeighbors(topn, botn, rightn, leftn);
			}
		}
	}
	GLCall(glGenTextures(1,&m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));


	GLCall(glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,m_Width,m_Height,0,GL_RGBA,GL_UNSIGNED_BYTE,m_LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D,0));
}

void Texture::Refresh(unsigned int speed, float mutation_rate, float influence_rate, float conserve_mut , float conserve_inf, bool colored)
{
	/*Affect Cells*/
	/*Refresh*/
	if (m_update >= speed) {
		m_update = 0;
	}
	else {
		m_update++;
	}

	for (int i = 0; i < m_Height; i++) {
		for (int j = 0; j < m_Width; j++) {
			if (!cells.at(i).at(j).isWater()) {
				SaveCell(i, j);
			}
		}
	}

	
	auto upcells = [this, speed, mutation_rate, influence_rate, conserve_mut, conserve_inf, colored](int hbegin, int hend) {
		for (int i = hbegin; i < hend; i++) {
			for (int j = 0; j < m_Width; j++) {
				if (!cells.at(i).at(j).isWater()) {
					int pos = i * m_Width * 4 + j * 4;
					if (m_update >= speed) {
						Texture::UpdateCell(i, j, mutation_rate, influence_rate, conserve_mut, conserve_inf);

						m_LocalBuffer[pos] = (unsigned char)cells.at(i).at(j).getColor(colored).red;
						m_LocalBuffer[pos + 1] = (unsigned char)cells.at(i).at(j).getColor(colored).green;
						m_LocalBuffer[pos + 2] = (unsigned char)cells.at(i).at(j).getColor(colored).blue;
					}
				}
			}
		}
	};

	std::vector<std::thread> threads;
	for (int split = 0; split < TEX_THREADS; split++) {
		threads.emplace_back(upcells, m_Height * split / TEX_THREADS, m_Height * (split + 1) / TEX_THREADS);
	}

	for (auto &thr : threads) {
		if(thr.joinable()) thr.join();
	}


	
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));

}

void Texture::UpdateCell(int i, int j, float mutation_rate, float influence_rate, float conserve_mut, float conserve_inf) {
	cells.at(i).at(j).createEvolution(mutation_rate, influence_rate, conserve_mut, conserve_inf);
}

void Texture::SaveCell(int i, int j) {
	cells.at(i).at(j).store();
}

Cell* Texture::getCell(int x, int y)
{ 
	if (x <= m_Width && y <= m_Height && x >= 0 && y >= 0) {
		return &cells.at(cells.size() - 1 - y).at(x);
	}
	return &cells.at(0).at(0);
}

