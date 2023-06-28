#include <ctime>
#include <iostream>
#include <vector>
#include <functional>
#include <numeric>

#include "Cell.h"

#define COLORED 0
#define BW_AVERAGE  1
#define BW_LIGHTNESS 2
#define BW_LUMINOSITY 3

Cell::Cell()
	:m_right(nullptr),m_top(nullptr),m_left(nullptr),m_bottom(nullptr),m_water(false), m_river(false), m_mount(false)
{
		int rsyn = getRandom(CELL_SYN_MIN, CELL_SYN_MAX);
		int rC = getRandom(CELL_C_MIN, CELL_C_MAX);
		int rV = getRandom(CELL_V_MIN, CELL_V_MAX);
		int rM = getRandom(CELL_M_MIN, CELL_M_MAX);
		int rG = getRandom(CELL_G_MIN, CELL_G_MAX);

		old.m_syntaxPos = m_syntaxPos = static_cast<SynPos>(rsyn);
		old.m_Csize = m_Csize = rC ;
		old.m_Vsize = m_Vsize = rV;
		old.m_Mtype = m_Mtype = rM;
		old.m_Gsize = m_Gsize = rG;

		m_CellV[0] = m_syntaxPos;
		m_CellV[1] = m_Csize;
		m_CellV[2] = m_Vsize;
		m_CellV[3] = m_Gsize;
		m_CellV[4] = m_Mtype;
}

Cell::Cell(const SynPos syntaxPos, const int Csize, const int Vsize, const int Mtype, const int Gsize)
	:m_right(nullptr), m_top(nullptr), m_left(nullptr), m_bottom(nullptr), m_water(false), m_river(false), m_mount(false)
{
	/*Bound checking*/

	cellAttrBind();
	store();

	m_CellV[0] = m_syntaxPos;
	m_CellV[1] = m_Csize;
	m_CellV[2] = m_Vsize;
	m_CellV[3] = m_Gsize;
	m_CellV[4] = m_Mtype;

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

RGBAstr Cell::getColor(const unsigned int colored) const
{

	//Get and normalize variables from 0-1
	//float norm_M = (m_Mtype - CELL_M_MIN)/ (CELL_M_MAX - CELL_M_MIN);
	//float norm_C = (m_Csize - CELL_C_MIN) / (CELL_C_MAX - CELL_C_MIN);
	//float norm_V = (m_Vsize - CELL_V_MIN) / (CELL_V_MAX - CELL_V_MIN);
	//float norm_G = (m_Gsize - CELL_G_MIN) / (CELL_G_MAX - CELL_G_MIN);
	RGBAstr colorrgb;

	constexpr unsigned int GC_MIN = static_cast<unsigned int>(CELL_G_MIN + CELL_C_MIN / 3.0f);
	constexpr unsigned int GC_MAX = static_cast<unsigned int>(CELL_G_MAX + CELL_C_MAX / 3.0f);

	constexpr unsigned int MC_MIN = static_cast<unsigned int>(CELL_M_MIN + CELL_C_MIN / 3.0f);
	constexpr unsigned int MC_MAX = static_cast<unsigned int>(CELL_M_MAX + CELL_C_MAX / 3.0f);

	constexpr unsigned int VC_MIN = static_cast<unsigned int>(CELL_C_MIN + CELL_C_MIN / 3.0f);
	constexpr unsigned int VC_MAX = static_cast<unsigned int>(CELL_C_MAX + CELL_C_MAX / 3.0f);

	const float red_norm =  ((m_Gsize + m_Csize / 3.0f) - GC_MIN)/(GC_MAX - GC_MIN);
	const float green_norm =((m_Mtype + m_Csize / 3.0f) - MC_MIN)/(MC_MAX - MC_MIN);
	const float blue_norm = ((m_Vsize + m_Csize / 3.0f) - VC_MIN)/(VC_MAX - VC_MIN);

	const unsigned int red_255 = (red_norm * 255);
	const unsigned int green_255 = (green_norm * 255);
	const unsigned int blue_255 = (blue_norm * 255);

	unsigned int grey_255;
	switch (colored) {
	case COLORED:
		colorrgb = { red_255, green_255, blue_255, 255 };
		break;
	case BW_AVERAGE:
		grey_255 = static_cast<unsigned int>((std::min({ red_255 , green_255 , blue_255 }) + std::max({ red_255, green_255, blue_255 })) / 2);
		colorrgb = { grey_255, grey_255, grey_255, 255 };
		break;
	case BW_LIGHTNESS:
		grey_255 = static_cast<unsigned int>((red_255 + green_255 + blue_255) / 3);
		colorrgb = { grey_255, grey_255, grey_255, 255 };
		break;
	case BW_LUMINOSITY:
		grey_255 = static_cast<unsigned int>(0.3f * red_255 + 0.59f * green_255 + 0.11 * blue_255);
		colorrgb = { grey_255, grey_255, grey_255, 255 };
		break;
	default:
		colorrgb = { 0,0,0 };
	}

	return colorrgb;
}

void Cell::mutate(const float rate)
{

	int rsynFlip = getRandom(0, 9);
	int rCFlip = getRandom(0, 9);
	int rVFlip = getRandom(0, 9);
	int rMFlip = getRandom(0, 9);
	int rGFlip = getRandom(0, 9);
	
	int globalchance = 5;
	if (rsynFlip >= globalchance){
		m_syntaxPos = static_cast<SynPos>(std::abs((m_syntaxPos + (int)rate) % CELL_SYN_MAX)) ;
	}
	else{
		m_syntaxPos = static_cast<SynPos>(std::abs((m_syntaxPos - (int)rate) % CELL_SYN_MAX));
	}
	if (rCFlip >= globalchance) {
		m_Csize += (int)rate;
	}
	else {
		m_Csize -= (int)rate;
	}
	if (rVFlip >= globalchance) {
		m_Vsize += (int)rate;
	}
	else {
		m_Vsize -= (int)rate;
	}
	if (rMFlip >= globalchance) {
		m_Mtype += (int)rate;
	}
	else {
		m_Mtype -= (int)rate;
	}
	if (rGFlip >= globalchance) {
		m_Gsize += (int)rate;
	}
	else {
		m_Gsize -= (int)rate;
	}
}

void Cell::createEvolution(const float mutation_rate, const float influence_rate,const float conserve_mut, float conserve_inf)
{

	float conserv_inf = getRandom(0.0f, 1.0f);
	float conserv_mut = getRandom(0.0f, 1.0f);

	bool mutated = false, influenced = false;

	if (this->isRiver()) conserve_inf -= 0.1f;
	else if (this->isMount()) conserve_inf += 0.1f;

	if (conserv_inf > conserve_inf) {
		influenced = true;
		//The neighbors of a cell make its contents move towards them more if they are correlated
		int aggSyn = 0, aggC = 0, aggV = 0, aggM = 0, aggG = 0;
		auto aggCalcOld = [this, &aggSyn, &aggC, &aggV, &aggM, &aggG, &influence_rate](const Cell* neighbor) {
			if (neighbor != nullptr) {
				const int neighborvec[5]{ neighbor->getOsyntaxPos() ,neighbor->getOCsize(),neighbor->getOVsize(),neighbor->getOGsize(),neighbor->getOMtype() };

				//Cosine sim A*B / ||A||*||B|| 
				const float cossim = std::transform_reduce(std::begin(m_CellV), std::end(m_CellV), std::begin(neighborvec), 0.01f) / (vecnorm5d(m_CellV) * vecnorm5d(neighborvec));
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
		m_Csize += aggC;
		m_Vsize += aggV;
		m_Mtype += aggM;
		m_Gsize += aggG;

	}

	if (conserv_mut > conserve_mut) {
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
