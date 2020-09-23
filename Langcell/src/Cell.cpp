#include <random>
#include <ctime>
#include <iostream>
#include <vector>
#include <functional>
#include <numeric>

#include "Cell.h"

float maxtot = -2;
float mintot = 2;
Cell::Cell(bool rand)
	:m_syntaxPos(SynPos::SVO), m_Csize(8), m_Vsize(3), m_Mtype(0), m_Gsize(0)
{
	if (rand) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> rsyn(0, 5);
		std::uniform_int_distribution<> rC(8, 100);
		std::uniform_int_distribution<> rV(3, 50);
		std::uniform_int_distribution<> rM(0, 50);
		std::uniform_int_distribution<> rG(0, 20);

		
		int randomsyntax = rsyn(gen);
		old.m_syntaxPos = m_syntaxPos = static_cast<SynPos>(randomsyntax);
		old.m_Csize = m_Csize = rC(gen);
		old.m_Vsize = m_Vsize = rV(gen);
		old.m_Mtype = m_Mtype = rM(gen);
		old.m_Gsize = m_Gsize = rG(gen);

		
	}
}

Cell::Cell(SynPos syntaxPos, float Csize, float Vsize, float Mtype, float Gsize)
{
	/*Bound checking*/
	old.m_syntaxPos = m_syntaxPos = static_cast<SynPos>(std::abs(syntaxPos % 6));
	old.m_Csize = m_Csize = Csize < 8 ? 8 : (Csize > 100 ? 100 : Csize);
	old.m_Vsize = m_Vsize = Vsize < 3 ? 3 : (Vsize > 50 ? 50 : Vsize);
	old.m_Mtype = m_Mtype = Mtype > 50 ? 50 : (Mtype < 0 ? 0 : Mtype);
	old.m_Gsize = m_Gsize = Gsize > 20 ? 20 : (Gsize < 0 ? 0 : Gsize);
}


Cell::~Cell(){}

RGBAstr Cell::getColor() const
{
	
	float red = m_Mtype;
	float green = m_Csize ;
	float blue = m_Vsize ;

	/*Normalize colors*/
	red = red  /50 ;
	green = (green - 8) / (100 - 8);
	blue = (blue - 3) / (50 - 3);

	unsigned int red_255 = red * (float)255;
	unsigned int green_255 = green * (float)255;
	unsigned int blue_255 = blue * (float)255;

	RGBAstr color = { red_255, green_255, blue_255, 255 };
	return color;
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
	if (rsynFlip(gen) >= globalchance + globalchance/2){
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

Cell* Cell::createEvolution(float rate,float closeness)
{
	std::vector<float> thiscell{static_cast<float>(this->getsyntaxPos()),this->getCsize(),this->getVsize(),this->getGsize(),this->getMtype()};
	//The neighbors of a cell make its contents move towards them more if they are correlated
	float cossimtop = -1;
	float aggSyn = 0, aggC = 0, aggV = 0, aggM = 0, aggG = 0;
	if (this->m_top != nullptr) {
		std::vector<float> top{ static_cast<float>(this->m_top->getOsyntaxPos()),this->m_top->getOCsize(),this->m_top->getOVsize(),this->m_top->getOGsize(),this->m_top->getOMtype() };
		cossimtop = static_cast<float>(std::inner_product(thiscell.begin(), thiscell.end(), top.begin(), 0.0f) / (vecnorm5d(thiscell)*vecnorm5d(top)));
		if (this->m_top->getOsyntaxPos() != m_syntaxPos) {
			aggSyn += ((m_syntaxPos < this->m_top->getOsyntaxPos()) ? +1 : -1) * cossimtop / (std::abs(this->m_top->getOsyntaxPos() - m_syntaxPos));
		}
		if (this->m_top->getOCsize() != m_Csize) {
			aggC += ((m_Csize < this->m_top->getOCsize()) ? +1 : -1) * cossimtop / (std::abs(this->m_top->getOCsize() - m_Csize));
		}
		if (this->m_top->getOVsize() != m_Vsize) {
			aggV += ((m_Vsize < this->m_top->getOVsize()) ? +1 : -1) * cossimtop / (std::abs(this->m_top->getOVsize() - m_Vsize));
		}
		if (this->m_top->getOMtype() != m_Mtype) {
			aggM += ((m_Mtype < this->m_top->getOMtype()) ? +1 : -1) * cossimtop / (std::abs(this->m_top->getOMtype() - m_Mtype));
		}
		if (this->m_top->getOGsize() != m_Gsize) {
			aggG += ((m_Gsize < this->m_top->getOGsize()) ? +1 : -1) * cossimtop / (std::abs(this->m_top->getOGsize() - m_Gsize));
		}

	}
	float cossimbot = -1;
	if (this->m_bottom != nullptr) {
		std::vector<float> bot{ static_cast<float>(this->m_bottom->getsyntaxPos()),this->m_bottom->getCsize(),this->m_bottom->getVsize(),this->m_bottom->getGsize(),this->m_bottom->getMtype() };
		cossimbot = static_cast<float>(std::inner_product(thiscell.begin(), thiscell.end(), bot.begin(), 0.0f) / (vecnorm5d(thiscell)*vecnorm5d(bot)));
		if (this->m_bottom->getsyntaxPos() != m_syntaxPos) {
			aggSyn += ((m_syntaxPos < this->m_bottom->getsyntaxPos()) ? +1 : -1) * cossimbot / (std::abs(this->m_bottom->getsyntaxPos() - m_syntaxPos));
		}
		if (this->m_bottom->getCsize() != m_Csize) {
			aggC += ((m_Csize < this->m_bottom->getCsize()) ? +1 : -1) * cossimbot / (std::abs(this->m_bottom->getCsize() - m_Csize));
		}
		if (this->m_bottom->getVsize() != m_Vsize) {
			aggV += ((m_Vsize < this->m_bottom->getVsize()) ? +1 : -1) * cossimbot / (std::abs(this->m_bottom->getVsize() - m_Vsize));
		}
		if (this->m_bottom->getMtype() != m_Mtype) {
			aggM += ((m_Mtype < this->m_bottom->getMtype()) ? +1 : -1) * cossimbot / (std::abs(this->m_bottom->getMtype() - m_Mtype));
		}
		if (this->m_bottom->getGsize() != m_Gsize) {
			aggG += ((m_Gsize < this->m_bottom->getGsize()) ? +1 : -1) * cossimbot / (std::abs(this->m_bottom->getGsize() - m_Gsize));
		}
	}
	float cossimleft = -1;
	if (this->m_left != nullptr) {
		std::vector<float> left{ static_cast<float>(this->m_left->getOsyntaxPos()),this->m_left->getOCsize(),this->m_left->getOVsize(),this->m_left->getOGsize(),this->m_left->getOMtype() };
		cossimleft = static_cast<float>(std::inner_product(thiscell.begin(), thiscell.end(), left.begin(), 0.0f) / (vecnorm5d(thiscell)*vecnorm5d(left)));
		if (this->m_left->getOsyntaxPos() != m_syntaxPos) {
			aggSyn += ((m_syntaxPos < this->m_left->getOsyntaxPos()) ? +1 : -1) * cossimleft / (std::abs(this->m_left->getOsyntaxPos() - m_syntaxPos));
		}
		if (this->m_left->getOCsize() != m_Csize) {
			aggC += ((m_Csize < this->m_left->getOCsize()) ? +1 : -1) * cossimleft / (std::abs(this->m_left->getOCsize() - m_Csize));
		}
		if (this->m_left->getOVsize() != m_Vsize) {
			aggV += ((m_Vsize < this->m_left->getOVsize()) ? +1 : -1) * cossimleft / (std::abs(this->m_left->getOVsize() - m_Vsize));
		}
		if (this->m_left->getOMtype() != m_Mtype) {
			aggM += ((m_Mtype < this->m_left->getOMtype()) ? +1 : -1) * cossimleft / (std::abs(this->m_left->getOMtype() - m_Mtype));
		}
		if (this->m_left->getOGsize() != m_Gsize) {
			aggG += ((m_Gsize < this->m_left->getOGsize()) ? +1 : -1) * cossimleft / (std::abs(this->m_left->getOGsize() - m_Gsize));
		}

	}
	float cossimright = -1;
	if (this->m_right != nullptr) {
		std::vector<float> right{ static_cast<float>(this->m_right->getsyntaxPos()),this->m_right->getCsize(),this->m_right->getVsize(),this->m_right->getGsize(),this->m_right->getMtype() };
		cossimright = static_cast<float>(std::inner_product(thiscell.begin(), thiscell.end(), right.begin(), 0.0f) / (vecnorm5d(thiscell)*vecnorm5d(right)));
		if (this->m_right->getsyntaxPos() != m_syntaxPos) {
			aggSyn += ((m_syntaxPos < this->m_right->getsyntaxPos()) ? +1 : 1) * cossimright / (std::abs(this->m_right->getsyntaxPos() - m_syntaxPos));
		}
		if (this->m_right->getCsize() != m_Csize) {
			aggC += ((m_Csize < this->m_right->getCsize()) ? +1 : -1) * cossimright / (std::abs(this->m_right->getCsize() - m_Csize));
		}
		if (this->m_right->getVsize() != m_Vsize) {
			aggV += ((m_Vsize < this->m_right->getVsize()) ? +1 : -1) * cossimright / (std::abs(this->m_right->getVsize() - m_Vsize));
		}
		if (this->m_right->getMtype() != m_Mtype) {
			aggM += ((m_Mtype < this->m_right->getMtype()) ? +1 : -1) * cossimright / (std::abs(this->m_right->getMtype() - m_Mtype));
		}
		if (this->m_right->getGsize() != m_Gsize) {
			aggG += ((m_Gsize < this->m_right->getGsize()) ? +1 : -1) * cossimright / (std::abs(this->m_right->getGsize() - m_Gsize));
		}
	}

	this->store();

	m_syntaxPos = static_cast<SynPos>(std::abs((m_syntaxPos + (int)aggSyn) % 6));
	m_Csize = m_Csize + aggC;
	m_Vsize = m_Vsize + aggV;
	m_Mtype = m_Mtype + aggM;
	m_Gsize = m_Gsize + aggG;

	mutate(rate);
	m_Csize = m_Csize < 8 ? 8 : (m_Csize > 100 ? 100 : m_Csize);
	m_Vsize = m_Vsize < 3 ? 3 : (m_Vsize > 50 ? 50 : m_Vsize);
	m_Mtype = m_Mtype > 50 ? 50 : (m_Mtype < 0 ? 0 : m_Mtype);
	m_Gsize = m_Gsize > 20 ? 20 : (m_Gsize < 0 ? 0 : m_Gsize);
	return nullptr;
}
void Cell::store() {
	old.m_Csize = m_Csize;
	old.m_Vsize = m_Vsize;
	old.m_Mtype = m_Mtype;
	old.m_Gsize = m_Gsize;
	old.m_syntaxPos = m_syntaxPos;
}