#include <random>
#include <ctime>
#include <iostream>
#include <vector>
#include <functional>
#include <numeric>

#include "Cell.h"

Cell::Cell()
	:m_right(nullptr),m_top(nullptr),m_left(nullptr),m_bottom(nullptr),m_water(false), m_river(false), m_mount(false)
{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> rsyn(0, 5);
		std::uniform_int_distribution<> rC(0, 255);
		std::uniform_int_distribution<> rV(0, 255);
		std::uniform_int_distribution<> rM(0, 255);
		std::uniform_int_distribution<> rG(0, 255);

		old.m_syntaxPos = m_syntaxPos = static_cast<SynPos>(rsyn(gen));
		old.m_Csize = m_Csize = static_cast<float>(rC(gen));
		old.m_Vsize = m_Vsize = static_cast<float>(rV(gen));
		old.m_Mtype = m_Mtype = static_cast<float>(rM(gen));
		old.m_Gsize = m_Gsize = static_cast<float>(rG(gen));

		this->m_CellV = std::vector<float>{ static_cast<float>(this->getsyntaxPos()), this->getCsize(), this->getVsize(), this->getGsize(), this->getMtype() };
}

Cell::Cell(SynPos syntaxPos, float Csize, float Vsize, float Mtype, float Gsize)
	:m_right(nullptr), m_top(nullptr), m_left(nullptr), m_bottom(nullptr), m_water(false), m_river(false), m_mount(false)
{
	/*Bound checking*/
	old.m_syntaxPos = m_syntaxPos = static_cast<SynPos>(std::abs(syntaxPos % 6)); 
	old.m_Csize = m_Csize = Csize < 0 ? 0 : (Csize > 255 ? 255 : Csize);
	old.m_Vsize = m_Vsize = Vsize < 0 ? 0 : (Vsize > 255 ? 255 : Vsize);
	old.m_Mtype = m_Mtype = Mtype > 255 ? 255 : (Mtype < 0 ? 0 : Mtype);
	old.m_Gsize = m_Gsize = Gsize > 255 ? 255: (Gsize < 1 ? 1 : Gsize);
	this->m_CellV = std::vector<float>{ static_cast<float>(this->getsyntaxPos()), this->getCsize(), this->getVsize(), this->getGsize(), this->getMtype() };
}


Cell::~Cell(){}

RGBAstr Cell::getColor() const
{

	float red = m_Mtype;
	float green = m_Csize;
	float blue = m_Vsize;

	/*Normalize colors*/
	red = red / 255 ;
	green = green / 255;
	blue  = blue  / 255;

	unsigned int red_255 = (unsigned int)(red * 255);
	unsigned int green_255 = (unsigned int)(green * 255);
	unsigned int blue_255 = (unsigned int)(blue * 255);

	RGBAstr colorrgb = { red_255, green_255, blue_255, 255 };
	return colorrgb;
}

void Cell::mutate(float rate)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> rsynFlip(0, 9);
	std::uniform_int_distribution<> rCFlip(0, 9);
	std::uniform_int_distribution<> rVFlip(0, 9);
	std::uniform_int_distribution<> rMFlip(0, 9);
	std::uniform_int_distribution<> rGFlip(0, 9);
	int globalchance = 5;
	if (rsynFlip(gen) >= globalchance){
		m_syntaxPos = static_cast<SynPos>(std::abs((m_syntaxPos + (int)rate) % 6)) ;
	}
	else{
		m_syntaxPos = static_cast<SynPos>(std::abs((m_syntaxPos - (int)rate) % 6));
	}
	if (rCFlip(gen) >= globalchance) {
		m_Csize = m_Csize + rate;
	}
	else {
		m_Csize = m_Csize - rate;
	}
	if (rVFlip(gen) >= globalchance) {
		m_Vsize = m_Vsize + rate;
	}
	else {
		m_Vsize = m_Vsize - rate;
	}
	if (rMFlip(gen) >= globalchance) {
		m_Mtype = m_Mtype + rate;
	}
	else {
		m_Mtype = m_Mtype - rate;
	}
	if (rGFlip(gen) >= globalchance) {
		m_Gsize = m_Gsize + rate;
	}
	else {
		m_Gsize = m_Gsize - rate;
	}
}

void Cell::createEvolution(float rate,int conserve)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> conserv(0, 1000);

	if (this->isRiver()) {
		conserve -= 100;
	}
	else if (this->isMount()) {
		conserve += 50;
	}

	if (conserv(gen) > conserve) {
		//The neighbors of a cell make its contents move towards them more if they are correlated
		float cossimtop = -1;
		float aggSyn = 0, aggC = 0, aggV = 0, aggM = 0, aggG = 0;
		auto aggCalcOld = [&aggSyn, &aggC, &aggV, &aggM, &aggG, this](Cell* neighbor) {
			if (neighbor != nullptr) {

				std::vector<float> neighborvec{ static_cast<float>(neighbor->getOsyntaxPos()),neighbor->getOCsize(),neighbor->getOVsize(),neighbor->getOGsize(),neighbor->getOMtype() };

				//A*B / ||A||*||B||
				float cossim = static_cast<float>(std::inner_product(m_CellV.begin(), m_CellV.end(), neighborvec.begin(), 0.0f) / (vecnorm5d(m_CellV) * vecnorm5d(neighborvec)));

				if (neighbor->getOsyntaxPos() != m_syntaxPos) {
					aggSyn += ((m_syntaxPos < neighbor->getOsyntaxPos()) ? +1 : -1) * cossim;
				}
				if (neighbor->getOCsize() != m_Csize) {
					aggC += ((m_Csize < neighbor->getOCsize()) ? +1 : -1) * cossim;
				}
				if (neighbor->getOVsize() != m_Vsize) {
					aggV += ((m_Vsize < neighbor->getOVsize()) ? +1 : -1) * cossim;
				}
				if (neighbor->getOMtype() != m_Mtype) {
					aggM += ((m_Mtype < neighbor->getOMtype()) ? +1 : -1) * cossim;
				}
				if (neighbor->getOGsize() != m_Gsize) {
					aggG += ((m_Gsize < neighbor->getOGsize()) ? +1 : -1) * cossim;
				}
			}
		};

		aggCalcOld(this->m_top);
		aggCalcOld(this->m_left);
		aggCalcOld(this->m_right);
		aggCalcOld(this->m_bottom);

		m_syntaxPos = static_cast<SynPos>(std::abs((m_syntaxPos + (int)aggSyn) % 6));
		m_Csize = m_Csize + aggC;
		m_Vsize = m_Vsize + aggV;
		m_Mtype = m_Mtype + aggM;
		m_Gsize = m_Gsize + aggG;


		mutate(rate);
		m_Csize = m_Csize < 0 ? 0 : (m_Csize > 255 ? 255 : m_Csize);
		m_Vsize = m_Vsize < 0 ? 0 : (m_Vsize > 255 ? 255 : m_Vsize);
		m_Mtype = m_Mtype > 255 ? 255 : (m_Mtype < 0 ? 0 : m_Mtype);
		m_Gsize = m_Gsize > 255 ? 255 : (m_Gsize < 1 ? 1 : m_Gsize);
	}
}
void Cell::store() {
	old.m_Csize = m_Csize;
	old.m_Vsize = m_Vsize;
	old.m_Mtype = m_Mtype;
	old.m_Gsize = m_Gsize;
	old.m_syntaxPos = m_syntaxPos;
}