/*
 * sims.h
 *
 *  Created on: Mar 9, 2017
 *      Author: chl
 */

#ifndef PROJ3_SIMS_H_
#define PROJ3_SIMS_H_

#include "grid.h"

#include <Eigen/SparseCholesky>
#include <deque>
#include <vector>

typedef unsigned int MGLpixel;
class Partical {
public:
	Partical(FSFloat x, FSFloat y, FSFloat c) :
			x(x), y(y), c(c) {
	}
	FSFloat x, y;
	FSFloat c;
};

class ShowPoint {
public:
	ShowPoint(FSszie x, FSszie y) :
			x(x), y(y) {
	}
	FSszie x, y;
};

inline MGLpixel Make_Pixel(int r, int g, int b) {
	return (r << 24) | (g << 16) | (b << 8) | 0xff;
}

class Simulation {
public:
	Simulation(int argc, char** argv);
	~Simulation();

	void Run();
	void SetP(FSszie x, FSszie y, FSFloat p);
	void SetUPH(FSszie x, FSszie y, FSFloat uph);
	void SetVNH(FSszie x, FSszie y, FSFloat vnh);
	void SetWall(FSszie x, FSszie y, bool set, bool rightHandSide);
	void ChangeDeltaT(FSFloat dt);
	void ChangeGrid(FSszie x, FSszie y);
	void ChangeMagnitude(FSszie m);
	void ChangeWindowMagnitude(FSszie wm);
	void ChangeNewPartsInterval(FSFloat pitv);
	void ChangePartsPerShow(FSszie pps);
	void SetMagnify();
	void SetDebugFlag();
	void SetShowPoint(FSszie x, FSszie y);

private:

	void InitGL();
	void InitPoissonMatrix();
	void InitBackground();
	void Tick();
	void Advection();
	void Poisson();
	void UpdateU();
	void Show();
	void ShowDebugGrid();
	void Display();

	void Interpolate(FSFloat x, FSFloat y, FSFloat& u, FSFloat& v);

private:
	FSszie getPos(FSszie x, FSszie y);
	FSszie getPixelPos(FSszie x, FSszie y);
	FSszie getMegaPos(FSszie x, FSszie y);
	int m_argc;
	char** m_argv;
	FSFloat m_delta_t, m_duration;
	FSszie m_x, m_y;
	FSszie m_width, m_height;
	bool m_Magnify;
	FSszie m_Magnitude;
	FSszie m_WindowsMag;
	MGLpixel* m_background;
	MGLpixel* m_pixel_data;
	MGLpixel* m_mag_background;
	MGLpixel* m_mag_pixel_data;
	int m_now_grid;
	Grid m_grid[2];
	Eigen::SparseMatrix<FSFloat> m_A;
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<FSFloat> > m_solver;
	std::deque<Partical> m_parts;
	bool m_debug;
	FSFloat m_pitv, m_lastshow;
	FSszie m_pps;
	FSFloat m_colorRatio, m_colorMul;
	std::vector<ShowPoint> m_sps;
};

#endif /* PROJ3_SIMS_H_ */
