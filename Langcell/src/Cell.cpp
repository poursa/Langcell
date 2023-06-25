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
		std::uniform_int_distribution<> rsyn(CELL_SYN_MIN, CELL_SYN_MAX);
		std::uniform_int_distribution<> rC(CELL_C_MIN, CELL_C_MAX);
		std::uniform_int_distribution<> rV(CELL_V_MIN, CELL_V_MAX);
		std::uniform_int_distribution<> rM(CELL_M_MIN, CELL_M_MAX);
		std::uniform_int_distribution<> rG(CELL_G_MIN, CELL_G_MAX);

		old.m_syntaxPos = m_syntaxPos = static_cast<SynPos>(rsyn(gen));
		old.m_Csize = m_Csize = static_cast<float>(rC(gen));
		old.m_Vsize = m_Vsize = static_cast<float>(rV(gen));
		old.m_Mtype = m_Mtype = static_cast<float>(rM(gen));
		old.m_Gsize = m_Gsize = static_cast<float>(rG(gen));

		this->m_CellV = std::vector<float>{ static_cast<float>(this->getsyntaxPos()), this->getCsize(), this->getVsize(), this->getGsize(), this->getMtype() };
}

Cell::Cell(const SynPos syntaxPos, const float Csize, const float Vsize, const float Mtype, const float Gsize)
	:m_right(nullptr), m_top(nullptr), m_left(nullptr), m_bottom(nullptr), m_water(false), m_river(false), m_mount(false)
{
	/*Bound checking*/

	cellAttrBind();
	store();
	this->m_CellV = std::vector<float>{ static_cast<float>(this->getsyntaxPos()), this->getCsize(), this->getVsize(), this->getGsize(), this->getMtype() };
}


Cell::~Cell(){}


/**
*
* -Syntax positions(SOV,SVO,...) Syn
* -Consonant inventory size (8...100) (lighter-darker) C
* -Vowel inventory size (3..50) (less blue...more blue)  V
* -Isolating,Agglutinative,fusional,polysynthetic(0...50)(less green- more green) M
* -Gender(0....20)(less red more red) G
*
**/

RGBAstr Cell::getColor(bool colored) const
{

	//Get and normalize variables from 0-1
	//float norm_M = (m_Mtype - CELL_M_MIN)/ (CELL_M_MAX - CELL_M_MIN);
	//float norm_C = (m_Csize - CELL_C_MIN) / (CELL_C_MAX - CELL_C_MIN);
	//float norm_V = (m_Vsize - CELL_V_MIN) / (CELL_V_MAX - CELL_V_MIN);
	//float norm_G = (m_Gsize - CELL_G_MIN) / (CELL_G_MAX - CELL_G_MIN);
	RGBAstr colorrgb;

	constexpr unsigned int GC_MIN = CELL_G_MIN + CELL_C_MIN / 3;
	constexpr unsigned int GC_MAX = CELL_G_MAX + CELL_C_MAX / 3;

	constexpr unsigned int MC_MIN = CELL_M_MIN + CELL_C_MIN / 3;
	constexpr unsigned int MC_MAX = CELL_M_MAX + CELL_C_MAX / 3;

	constexpr unsigned int VC_MIN = CELL_C_MIN + CELL_C_MIN / 3;
	constexpr unsigned int VC_MAX = CELL_C_MAX + CELL_C_MAX / 3;

	const float red_norm =  ((m_Gsize + m_Csize /3) - GC_MIN)/(GC_MAX - GC_MIN);
	const float green_norm =((m_Mtype + m_Csize /3) - MC_MIN)/(MC_MAX - MC_MIN);
	const float blue_norm = ((m_Vsize + m_Csize /3) - VC_MIN)/(VC_MAX - VC_MIN);

	const unsigned int red_255 = (red_norm * 255);
	const unsigned int green_255 = (green_norm * 255);
	const unsigned int blue_255 = (blue_norm * 255);

	if (colored) {
		colorrgb = { red_255, green_255, blue_255, 255 };
	}
	else {
		unsigned int grey_255 = (unsigned int)((red_255 + green_255 + blue_255) / 3);
		colorrgb = { grey_255, grey_255, grey_255, 255 };
	}
	return colorrgb;
}

void Cell::mutate(const float rate)
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
		m_syntaxPos = static_cast<SynPos>(std::abs((m_syntaxPos + (int)rate) % CELL_SYN_MAX)) ;
	}
	else{
		m_syntaxPos = static_cast<SynPos>(std::abs((m_syntaxPos - (int)rate) % CELL_SYN_MAX));
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

void Cell::createEvolution(const float mutation_rate, const float influence_rate,const float conserve_mut, float conserve_inf)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> conserv_inf(0, 1);
	std::uniform_real_distribution<> conserv_mut(0, 1);
	bool mutated = false, influenced = false;

	if (this->isRiver()) conserve_inf -= 0.1f;
	else if (this->isMount()) conserve_inf += 0.1f;

	if (conserv_inf(gen) > conserve_inf) {
		influenced = true;
		//The neighbors of a cell make its contents move towards them more if they are correlated
		float aggSyn = 0, aggC = 0, aggV = 0, aggM = 0, aggG = 0;
		auto aggCalcOld = [this, &aggSyn, &aggC, &aggV, &aggM, &aggG, &influence_rate](const Cell* neighbor) {
			if (neighbor != nullptr) {
				const std::vector<float> neighborvec{ static_cast<float>(neighbor->getOsyntaxPos()) ,neighbor->getOCsize(),neighbor->getOVsize(),neighbor->getOGsize(),neighbor->getOMtype() };

				//Cosine sim A*B / ||A||*||B|| 
				const float cossim = static_cast<float>(std::transform_reduce(m_CellV.begin(), m_CellV.end(), neighborvec.begin(), 0.01f) / (vecnorm5d(m_CellV) * vecnorm5d(neighborvec)));
				const float cos_rate = cossim * influence_rate;

				aggSyn += (neighbor->getOsyntaxPos() - m_syntaxPos) * cos_rate;
				aggC += (neighbor->getOCsize() - m_Csize) * cos_rate;
				aggV += (neighbor->getOVsize() - m_Vsize) * cos_rate;
				aggM += (neighbor->getOMtype() - m_Mtype) * cos_rate;
				aggG += (neighbor->getOGsize() - m_Gsize) * cos_rate;
			}
		};

		aggCalcOld(this->m_top);
		aggCalcOld(this->m_left);
		aggCalcOld(this->m_right);
		aggCalcOld(this->m_bottom);

		m_syntaxPos = static_cast<SynPos>(std::abs((m_syntaxPos + (int)aggSyn) % CELL_SYN_MAX));
		m_Csize = m_Csize + aggC;
		m_Vsize = m_Vsize + aggV;
		m_Mtype = m_Mtype + aggM;
		m_Gsize = m_Gsize + aggG;

	}

	if (conserv_mut(gen) > conserve_mut) {
		mutated = true;
		mutate(mutation_rate);
	}

	if (influenced || mutated) {
		cellAttrBind();
	}

}
void Cell::store() {
	old.m_Csize = m_Csize;
	old.m_Vsize = m_Vsize;
	old.m_Mtype = m_Mtype;
	old.m_Gsize = m_Gsize;
	old.m_syntaxPos = m_syntaxPos;
}

void Cell::cellAttrBind()
{
	m_syntaxPos = static_cast<SynPos>(std::abs(m_syntaxPos % CELL_SYN_MAX));
	m_Csize = m_Csize < CELL_C_MIN ? CELL_C_MIN : (m_Csize > CELL_C_MAX ? CELL_C_MAX : m_Csize);
	m_Vsize = m_Vsize < CELL_V_MIN ? CELL_V_MIN : (m_Vsize > CELL_V_MAX ? CELL_V_MAX : m_Vsize);
	m_Mtype = m_Mtype < CELL_M_MIN ? CELL_M_MIN : (m_Mtype > CELL_M_MAX ? CELL_M_MAX : m_Mtype);
	m_Gsize = m_Gsize < CELL_G_MIN ? CELL_G_MIN : (m_Gsize > CELL_G_MAX ? CELL_G_MAX : m_Gsize);
}
