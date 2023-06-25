#pragma once

/**
*
* A cell has the following characteristics
* Attributes:
* -Syntax positions(SOV,SVO,...)
* -Consonant inventory size (8...100) 
* -Vowel inventory size (3..50)
* -Isolating,Agglutinative,fusional,polysynthetic(0...50)
* -Gender(0....20)
*
**/




enum SynPos {
	SOV, SVO, VOS, VSO, OSV, OVS, SYNPOS_MAX = OVS
};

#define CELL_SYN_MAX SYNPOS_MAX
#define CELL_C_MAX 255
#define CELL_V_MAX 255
#define CELL_M_MAX 255
#define CELL_G_MAX 255

#define CELL_SYN_MIN 0
#define CELL_C_MIN 0
#define CELL_V_MIN 0
#define CELL_M_MIN 0
#define CELL_G_MIN 1


struct RGBAstr {
	unsigned int red, green, blue, alpha;
};


class Cell {
private:
	SynPos m_syntaxPos;
	float m_Csize,m_Vsize,m_Mtype,m_Gsize;
	Cell* m_top, *m_bottom, *m_right, *m_left;
	bool m_water,m_river,m_mount;
	struct {
		SynPos m_syntaxPos;
		float m_Csize, m_Vsize, m_Mtype, m_Gsize;
	} old;
	std::vector<float> m_CellV;
public:
	Cell();
	Cell(const SynPos m_syntaxPos, const float m_Csize, const float m_Vsize, const float m_Mtype, const float m_Gsize);
	~Cell();

	inline float vecnorm5d(std::vector<float> a) { return std::sqrtf(a[0]* a[0] + a[1]* a[1] + a[2]* a[2] + a[3]* a[3] + a[4]*a[4]); }
	//inline void setCoords(unsigned int x, unsigned int y) { m_x = x; m_y = y; }
	inline void setNeighbors(Cell* top, Cell* bottom, Cell* right, Cell* left) { m_top = top; m_bottom = bottom; m_right = right; m_left = left; }
	inline void setWater(bool water) { m_water = water; }
	inline void setRiver(bool river) { m_river = river; }
	inline void setMount(bool mount) { m_mount = mount; }

	void mutate(float rate);
	void createEvolution(const float mutation_rate, const float influence_rate, const float conserve_mut, float conserve_inf);
	void store();
	void cellAttrBind();

	RGBAstr getColor(bool colored) const;
	bool isMount() const { return m_mount; }
	bool isWater() const { return m_water; }
	bool isRiver() const { return m_river; }

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