/*
 * sims.h
 *
 *  Created on: Mar 9, 2017
 *      Author: chl
 */

#ifndef PROJ3_SIMS_H_
#define PROJ3_SIMS_H_

#include "grid.h"

class Simulation {
public:
	Simulation() :
			m_delta_t(0), m_x(0), m_y(0) {

	}

	Simulation(FSszie x, FSszie y, FSFloat dt) :
			m_delta_t(dt), m_x(x), m_y(y), m_grid(x, y) {

	}

	void Run();
	void Set(FSszie x, FSszie y, FSFloat p, const FSF2D & u);
	void SetWall(FSszie x, FSszie y, bool set, bool rightHandSide);
	void Show(); //RunCallShow
	bool ChangeDeltaT(FSFloat dt);

private:
	FSFloat m_delta_t;
	FSszie m_x, m_y;
	Grid m_grid;
};

#endif /* PROJ3_SIMS_H_ */
