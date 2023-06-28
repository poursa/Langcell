#pragma once
#include <random>
#include <mutex>

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
	int m_Csize,m_Vsize,m_Mtype,m_Gsize;
	Cell* m_top, *m_bottom, *m_right, *m_left;
	bool m_water,m_river,m_mount;
	struct {
		SynPos m_syntaxPos;
		int m_Csize, m_Vsize, m_Mtype, m_Gsize;
	} old;
	int m_CellV[5];

	static inline std::random_device rd;
	static std::mt19937 gen;
	
	static int getRandom(int a, int b) {
		static std::mutex mut;
		std::lock_guard guard(mut);
		static std::uniform_int_distribution<int> distr;

		return distr(gen, std::uniform_int_distribution<int>::param_type(a,b));
	}
	static float getRandom(float a, float b) {
		static std::uniform_real_distribution<float> distr;

		return distr(gen, std::uniform_real_distribution<float>::param_type(a, b));
	}


public:
	Cell();
	Cell(const SynPos m_syntaxPos, const int m_Csize, const int m_Vsize, const int m_Mtype, const int m_Gsize);
	~Cell();

	//inline float vecnorm5d(const std::vector<int> &a) { return std::sqrtf(a[0]* a[0] + a[1]* a[1] + a[2]* a[2] + a[3]* a[3] + a[4]*a[4]); }
	inline float vecnorm5d(const int (&a)[5]) { return std::sqrtf(a[0] * a[0]*1.0f + a[1] * a[1] + a[2] * a[2] + a[3] * a[3] + a[4] * a[4]); }
	//inline void setCoords(unsigned int x, unsigned int y) { m_x = x; m_y = y; }
	inline void setNeighbors(Cell* top, Cell* bottom, Cell* right, Cell* left) { m_top = top; m_bottom = bottom; m_right = right; m_left = left; }
	inline void setWater(bool water) { m_water = water; }
	inline void setRiver(bool river) { m_river = river; }
	inline void setMount(bool mount) { m_mount = mount; }

	void mutate(float rate);
	void createEvolution(const float mutation_rate, const float influence_rate, const float conserve_mut, float conserve_inf);
	void store();
	void cellAttrBind();

	RGBAstr getColor(const unsigned int colored) const;
	bool isMount() const { return m_mount; }
	bool isWater() const { return m_water; }
	bool isRiver() const { return m_river; }

	SynPos getsyntaxPos()const	{ return m_syntaxPos; }
	int getCsize()const { return m_Csize; }
	int getVsize()const { return m_Vsize; }
	int getMtype()const { return m_Mtype; }
	int getGsize()const { return m_Gsize; }

	SynPos getOsyntaxPos()const { return old.m_syntaxPos; }
	int getOCsize()const { return old.m_Csize; }
	int getOVsize()const { return old.m_Vsize; }
	int getOMtype()const { return old.m_Mtype; }
	int getOGsize()const { return old.m_Gsize; }
};

inline std::mt19937 Cell::gen(rd());