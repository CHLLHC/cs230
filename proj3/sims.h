/*
 * sims.h
 *
 *  Created on: Mar 9, 2017
 *      Author: chl
 */

#ifndef PROJ3_SIMS_H_
#define PROJ3_SIMS_H_

#include "grid.h"

typedef unsigned int MGLpixel;

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
	void SetVPH(FSszie x, FSszie y, FSFloat vph)
	void SetWall(FSszie x, FSszie y, bool set, bool rightHandSide);
	void ChangeDeltaT(FSFloat dt);
	void ChangeGrid(FSszie x, FSszie y);
	void ChangeDuration(FSFloat t);
	void SetMagnify();

private:

	void InitGL();
	void Tick();
	void Advection();
	void Poisson();
	void GetNewU();
	void Show();
	void Display();

	void Interpolate(FSFloat x, FSFloat y, FSFloat& u, FSFloat& v);

private:
	FSszie getPos(FSszie x, FSszie y);
	FSszie getMegaPos(FSszie x, FSszie y);
	int m_argc;
	char** m_argv;
	FSFloat m_delta_t, m_duration;
	FSszie m_x, m_y;
	FSszie m_width, m_height;
	bool m_Magnify;
	MGLpixel* m_pixel_data;
	MGLpixel* m_mag_pixel_data;
	int m_now_grid;
	Grid m_grid[2];
};

#endif /* PROJ3_SIMS_H_ */
