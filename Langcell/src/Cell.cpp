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
		old.m_Csize = m_Csize = static_cast<float>(rC(gen));
		old.m_Vsize = m_Vsize = static_cast<float>(rV(gen));
		old.m_Mtype = m_Mtype = static_cast<float>(rM(gen));
		old.m_Gsize = m_Gsize = static_cast<float>(rG(gen));

		
	}
}

Cell::Cell(SynPos syntaxPos, float Csize, float Vsize, float Mtype, float Gsize)
{
	/*Bound checking*/
	old.m_syntaxPos = m_syntaxPos = static_cast<SynPos>(std::abs(syntaxPos % 6)); //3 bits
	old.m_Csize = m_Csize = Csize < 8 ? 8 : (Csize > 100 ? 100 : Csize); //5
	old.m_Vsize = m_Vsize = Vsize < 3 ? 3 : (Vsize > 50 ? 50 : Vsize); //4
	old.m_Mtype = m_Mtype = Mtype > 50 ? 50 : (Mtype < 0 ? 0 : Mtype); //4
	old.m_Gsize = m_Gsize = Gsize > 20 ? 20 : (Gsize < 0 ? 0 : Gsize); //4
}


Cell::~Cell(){}

RGBAstr Cell::getColor() const
{

	float red = m_Mtype;
	float green = m_Csize;
	float blue = m_Vsize ;

	///*Normalize colors*/
	red = red  /50 ;
	green = (green - 8) / (100 - 8);
	blue  = (blue - 3) / (50 - 3);

	unsigned int red_255 = red * 255.f;
	unsigned int green_255 = green * 255.f;
	unsigned int blue_255 = blue * 255.f;

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

Cell* Cell::createEvolution(float rate)
{
	std::vector<float> thiscell{static_cast<float>(this->getsyntaxPos()),this->getCsize(),this->getVsize(),this->getGsize(),this->getMtype()};
	//The neighbors of a cell make its contents move towards them more if they are correlated
	float cossimtop = -1;
	float aggSyn = 0, aggC = 0, aggV = 0, aggM = 0, aggG = 0;
	auto aggCalcOld = [&aggSyn, &aggC, &aggV, &aggM, &aggG,&thiscell, this](Cell* neighbor) {
		if (neighbor != nullptr) {
			std::vector<float> top{ static_cast<float>(neighbor->getOsyntaxPos()),neighbor->getOCsize(),neighbor->getOVsize(),neighbor->getOGsize(),neighbor->getOMtype() };
			float cossim = static_cast<float>(std::inner_product(thiscell.begin(), thiscell.end(), top.begin(), 0.0f) / (vecnorm5d(thiscell) * vecnorm5d(top)));
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
	auto aggCalc = [&aggSyn, &aggC, &aggV, &aggM, &aggG, &thiscell, this](Cell* neighbor) {
		if (neighbor != nullptr) {
			std::vector<float> top{ static_cast<float>(neighbor->getsyntaxPos()),neighbor->getCsize(),neighbor->getVsize(),neighbor->getGsize(),neighbor->getMtype() };
			float cossim = static_cast<float>(std::inner_product(thiscell.begin(), thiscell.end(), top.begin(), 0.0f) / (vecnorm5d(thiscell) * vecnorm5d(top)));
			if (neighbor->getsyntaxPos() != m_syntaxPos) {
				aggSyn += ((m_syntaxPos < neighbor->getsyntaxPos()) ? +1 : -1) * cossim;
			}
			if (neighbor->getCsize() != m_Csize) {
				aggC += ((m_Csize < neighbor->getCsize()) ? +1 : -1) * cossim;
			}
			if (neighbor->getVsize() != m_Vsize) {
				aggV += ((m_Vsize < neighbor->getVsize()) ? +1 : -1) * cossim;
			}
			if (neighbor->getMtype() != m_Mtype) {
				aggM += ((m_Mtype < neighbor->getMtype()) ? +1 : -1) * cossim;
			}
			if (neighbor->getGsize() != m_Gsize) {
				aggG += ((m_Gsize < neighbor->getGsize()) ? +1 : -1) * cossim;
			}

		}
	};
	aggCalcOld(this->m_top);
	aggCalcOld(this->m_left);
	aggCalc(this->m_right);
	aggCalc(this->m_bottom);
	
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