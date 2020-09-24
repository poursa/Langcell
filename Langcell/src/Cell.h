#pragma once

/**
*
* A cell has the following characteristics
* Attributes:
* -Syntax positions(SOV,SVO,...)(less yellow- more yellow)
* -Consonant inventory size (8...100) (lighter-darker)
* -Vowel inventory size (3..50) (less blue...more blue)
* -Isolating,Agglutinative,fusional,polysynthetic(0...50)(less green more green)
* -Gender(0....20)(less red more red)
*
* 
*
*
**/



enum SynPos {
	SOV, SVO, VOS, VSO, OSV, OVS
};
struct RGBAstr {
	unsigned int red, green, blue, alpha;
};


class Cell {
private:
	SynPos m_syntaxPos;
	float m_Csize,m_Vsize,m_Mtype,m_Gsize;
	Cell* m_top, *m_bottom, *m_right, *m_left;
	bool m_water;
	struct {
		SynPos m_syntaxPos;
		float m_Csize, m_Vsize, m_Mtype, m_Gsize;
	} old;

public:
	Cell();
	Cell(SynPos m_syntaxPos, float m_Csize, float m_Vsize, float m_Mtype, float m_Gsize);
	~Cell();

	inline float vecnorm5d(std::vector<float> a) { return std::sqrtf(a[0]* a[0] + a[1]* a[1] + a[2]* a[2] + a[3]* a[3] + a[4]*a[4]); }
	//inline void setCoords(unsigned int x, unsigned int y) { m_x = x; m_y = y; }
	inline void setNeighbors(Cell* top, Cell* bottom, Cell* right, Cell* left) { m_top = top; m_bottom = bottom; m_right = right; m_left = left; }
	inline void setWater(bool water) { m_water = water; }

	void mutate(float rate);
	Cell* createEvolution(float rate);
	void store();

	RGBAstr getColor() const;

	bool isWater() const { return m_water; }
	SynPos getsyntaxPos()const	{ return m_syntaxPos; }
	float getCsize()const { return m_Csize; }
	float getVsize()const { return m_Vsize; }
	float getMtype()const { return m_Mtype; }
	float getGsize()const { return m_Gsize; }
	SynPos getOsyntaxPos()const { return old.m_syntaxPos; }
	float getOCsize()const { return old.m_Csize; }
	float getOVsize()const { return old.m_Vsize; }
	float getOMtype()const { return old.m_Mtype; }
	float getOGsize()const { return old.m_Gsize; }
};