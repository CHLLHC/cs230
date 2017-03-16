/*
 * grid.h
 *
 *  Created on: Mar 9, 2017
 *      Author: chl
 */

#ifndef PROJ3_GRID_H_
#define PROJ3_GRID_H_

#include <cstdint>
#include <vector>


typedef double FSFloat;
typedef uint32_t FSszie;

class Cell {
public:
	Cell() :
			m_uph(0), m_vph(0), m_p(1), m_wall_right(false), m_wall_bottom(
					false), m_fixP(false), m_fixUPH(false), m_fixVPH(false) {
	}
	Cell(const Cell& o) :
			m_uph(o.m_uph), m_vph(o.m_vph), m_p(o.m_p), m_wall_right(
					o.m_wall_right), m_wall_bottom(o.m_wall_bottom), m_fixP(
					o.m_fixP), m_fixUPH(o.m_fixUPH), m_fixVPH(o.m_fixVPH) {
	}

	FSFloat m_uph, m_vph; //u(i+1/2,j) , v(i,j+1/2)
	FSFloat m_p;
	bool m_wall_right, m_wall_bottom;
	bool m_fixP, m_fixUPH, m_fixVPH;
};

class Grid {
public:
	Grid() :
			m_x(0), m_y(0) {

	}
	Grid(FSszie x, FSszie y) :
			m_x(x), m_y(y) {
		for (FSszie i = 0; i < m_x; ++i)
			for (FSszie j = 0; j < m_y; ++j) {
				m_array.push_back(Cell());
			}
	}
	Grid(const Grid& o) :
			m_x(o.m_x), m_y(o.m_y) {
		m_array.clear();
		for (FSszie i = 0; i < o.m_array.size(); ++i) {
			m_array.push_back(o.m_array[i]);
		}
	}

	FSszie Locate(FSszie x, FSszie y) {
		return y * m_x + x;
	}

	FSszie m_x, m_y;
	std::vector<Cell> m_array;
}
;

#endif /* PROJ3_GRID_H_ */
