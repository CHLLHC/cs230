/*
 * grid.h
 *
 *  Created on: Mar 9, 2017
 *      Author: chl
 */

#ifndef PROJ3_GRID_H_
#define PROJ3_GRID_H_

#include <Eigen/Dense>
#include <stdint.h>
#include <vector>

using namespace Eigen;

typedef double FSFloat;
typedef Vector2d FSF2D;
typedef u_int32_t FSszie;

class Cell {
public:
	Cell() :
			m_u(0, 0), m_p(0), m_wall_right(false), m_wall_bottom(false) {
	}
	Cell(FSF2D u, FSFloat p) :
			m_u(u), m_p(p), m_wall_right(false), m_wall_bottom(false) {
	}
	Cell(const Cell& o) :
			m_u(o.m_u), m_p(o.m_p), m_wall_right(o.m_wall_right), m_wall_bottom(
					o.m_wall_bottom) {
	}

	FSF2D m_u;
	FSFloat m_p;
	bool m_wall_right, m_wall_bottom;
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
		for (FSszie i = 0; i < o.m_array.size(); ++i) {
			m_array[i] = o.m_array[i];
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
