#include "Texture.h"
#include <iostream>
#include "Cell.h"

#include "stb_image/stb_image.h"
Texture::Texture(const std::string & path)
	:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0), m_update(0)
{
	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);
	std::cout << m_Height << m_Width << std::endl;
	for (int i = 0; i < m_Height; i++) {
		std::vector<Cell*> row;
		for (int j = 0; j < m_Width; j += 1) {
			int pos = i * m_Width * 4 + j*4;
			Cell* cell = new Cell(true);
			cell->setCoords(i, j);
			if (m_LocalBuffer[pos + 3] == 255) {
				m_LocalBuffer[pos] = (unsigned char)cell->getColor().red;
				m_LocalBuffer[pos + 1] = (unsigned char)cell->getColor().green;
				m_LocalBuffer[pos + 2] = (unsigned char)cell->getColor().blue;
				cell->setWater(false);
			}
			else {
				cell->setWater(true);
			}
			row.push_back(cell);
		}
		cells.push_back(row);
	}
	/*Set Neighobrs*/
	for (int i = 0; i < m_Height; i++) {
		for (int j = 0; j < m_Width; j++) {
			int pos = i * m_Height * 4 + j * 4;
			if (!cells.at(i).at(j)->isWater()) {
				Cell* topn, *botn, *rightn, *leftn;
				if (i == 0) {
					topn = nullptr;
					botn = cells.at(i + 1).at(j);
					if (botn->isWater()) {
						botn = nullptr;
					}
				}else if (i == m_Height - 1) {
					topn = cells.at(i - 1).at(j);
					botn = nullptr;
					if (topn->isWater()) {
						topn = nullptr;
					}
				}else{
					topn = cells.at(i - 1).at(j);
					botn = cells.at(i + 1).at(j);
					if (topn->isWater()) {
						topn = nullptr;
					}
					if (botn->isWater()) {
						botn = nullptr;
					}
				}
				if (j == 0) {
					rightn = cells.at(i).at(j + 1);
					leftn = nullptr;
					if (rightn->isWater()) {
						rightn = nullptr;
					}
				}else if (j == m_Width - 1 ){
					rightn = nullptr;
					leftn = cells.at(i).at(j - 1);
					if (leftn->isWater()) {
						leftn = nullptr;
					}
				}else{
					rightn = cells.at(i).at(j + 1);
					leftn = cells.at(i).at(j - 1);
					if (rightn->isWater()) {
						rightn = nullptr;
					}
					if (leftn->isWater()) {
						leftn = nullptr;
					}
				}
				cells.at(i).at(j)->setNeighbors(topn, botn, rightn, leftn);
			}
			if (m_LocalBuffer[pos + 3] == 255) {
				m_LocalBuffer[pos] = (unsigned char)cells.at(i).at(j)->getColor().red;
				m_LocalBuffer[pos + 1] = (unsigned char)cells.at(i).at(j)->getColor().green;
				m_LocalBuffer[pos + 2] = (unsigned char)cells.at(i).at(j)->getColor().blue;
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

	if (m_LocalBuffer) {
		stbi_image_free(m_LocalBuffer);
	}
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

void Texture::Refresh(unsigned int speed,float mutation)
{
	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	
	/*Affect Cells*/
	/*Refresh*/
	if (m_update >= speed) {
		m_update = 0;
	}
	else {
		m_update++;
	}
	for (int i = 0; i < m_Height; i++) {
		for (int j = 0; j < m_Width; j += 1) {
			if (!cells.at(i).at(j)->isWater()) {
				int pos = i * m_Width * 4 + j * 4;
				if (m_update >= speed) {
					cells.at(i).at(j)->createEvolution(mutation);
				}
				if (m_LocalBuffer[pos + 3] == 255) {
					m_LocalBuffer[pos] = (unsigned char)cells.at(i).at(j)->getColor().red;
					m_LocalBuffer[pos + 1] = (unsigned char)cells.at(i).at(j)->getColor().green;
					m_LocalBuffer[pos + 2] = (unsigned char)cells.at(i).at(j)->getColor().blue;
				}
			}
		}
	}

	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	
	if (m_LocalBuffer) {
		stbi_image_free(m_LocalBuffer);
	}
}
