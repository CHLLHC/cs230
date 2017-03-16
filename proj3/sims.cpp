/*
 * sims.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: chl
 */

#include "sims.h"
#include <cstring>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>

Simulation::Simulation(int argc, char** argv) :
		m_argc(argc), m_argv(argv), m_delta_t(0.01), m_duration(0), m_x(320), m_y(
				240), m_width(320), m_height(240), m_Magnify(false), m_now_grid(
				0) {
	m_pixel_data = new MGLpixel[m_width * m_height];
	m_mag_pixel_data = new MGLpixel[4 * m_width * 4 * m_height];
}

Simulation::~Simulation() {
	delete[] m_pixel_data;
	delete[] m_mag_pixel_data;
}

void Simulation::Display() {
	if (m_Magnify) {
		glViewport(0, 0, 4 * m_width, 4 * m_height);
	} else {
		glViewport(0, 0, m_width, m_height);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRasterPos2f(0, 0);
	if (m_Magnify) {
		for (FSszie j = 0; j < m_height; ++j)
			for (FSszie i = 0; i < m_width; ++i) {
				for (FSszie y = j * 4; y < (j + 1) * 4; ++y)
					for (FSszie x = i * 4; x < (i + 1) * 4; ++x) {
						m_mag_pixel_data[getMegaPos(x, y)] =
								m_pixel_data[getPos(i, j)];
					}
			}
		glDrawPixels(4 * m_width, 4 * m_height, GL_RGBA,
		GL_UNSIGNED_INT_8_8_8_8, (GLvoid*) m_mag_pixel_data);
	} else {
		glDrawPixels(m_width, m_height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
				(GLvoid*) m_pixel_data);
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glFlush();
}

void Simulation::Run() {
	InitGL();
	InitPoissonMatrix();

	bool a = false;
	while (1) {
		if (a) {
			std::cout << "===Tack" << std::endl;
		} else {
			std::cout << "Tick===" << std::endl;
		}
		a = !a;
		Tick();
	}

}

void Simulation::Tick() {
	Advection();
	Poisson();
	UpdateU();
	Show();
}

void Simulation::Advection() {
	int _new_grid = 1 - m_now_grid;
	for (FSszie i = 0; i < m_x; i++) {
		for (FSszie j = 0; j < m_y; ++j) {
			if (!m_grid[m_now_grid].m_array[getPos(i, j)].m_fixUPH) {
//				//L1-+- R1
//				//   U
//				//L2-+- R2
//				FSFloat L1 = 0, L2 = 0, R1 = 0, R2 = 0;
//				if (j + 1 < m_y) {
//					L1 = m_grid[m_now_grid].m_array[getPos(i, j + 1)].m_vph;
//				}
//				L2 = m_grid[m_now_grid].m_array[getPos(i, j)].m_vph;
//				if ((i + 1 < m_x) && (j + 1 < m_y)) {
//					L2 = m_grid[m_now_grid].m_array[getPos(i + 1, j + 1)].m_vph;
//				}
//				if (i + 1 < m_x) {
//					L1 = m_grid[m_now_grid].m_array[getPos(i + 1, j)].m_vph;
//				}
//				FSFloat vUPH = (L1 + R1 + L2 + R2) / 4;
				FSFloat ug, vg;
				Interpolate(i + 0.5, j, ug, vg);
				FSFloat px = i + 0.5 - m_delta_t * ug;
				FSFloat py = j - m_delta_t * vg;
				FSFloat up, vp;
				Interpolate(px, py, up, vp);
				m_grid[_new_grid].m_array[getPos(i, j)].m_uph = up;
			}
			if (!m_grid[m_now_grid].m_array[getPos(i, j)].m_fixVPH) {
				FSFloat ug, vg;
				Interpolate(i, j - 0.5, ug, vg);
				FSFloat px = i - m_delta_t * ug;
				FSFloat py = j - 0.5 - m_delta_t * vg;
				FSFloat up, vp;
				Interpolate(px, py, up, vp);
				m_grid[_new_grid].m_array[getPos(i, j)].m_vph = vp;
			}

		}
	}
}

void Simulation::Interpolate(FSFloat x, FSFloat y, FSFloat& u, FSFloat& v) {

	//For x
	// 1|4
	//--+--
	// 2|3

	FSszie x1, x2, x3, x4, y1, y2, y3, y4;
	x2 = floor(x - 0.5);
	y2 = floor(y);

	x1 = x2;
	x3 = x2 + 1;
	x4 = x3;

	y1 = y2 + 1;
	y3 = y2;
	y4 = y1;

	//alpha for x, beta for y
	FSFloat alpha = x - x2 + 0.5;
	FSFloat beta = y - y2;

	bool w1 = true, w2 = true, w3 = true, w4 = true;
	//will only block by bottom wall of P4

	if (beta < 0.5) {
		w2 = w3 = false;
		if ((x4 >= 0) && (y4 >= 0) && (x4 < m_x) && (y4 < m_y)) {
			if (!m_grid[m_now_grid].m_array[getPos(x4, y4)].m_wall_bottom) {
				w1 = w4 = false;
			}
		}
	} else {
		w1 = w4 = false;
		if ((x4 >= 0) && (y4 >= 0) && (x4 < m_x) && (y4 < m_y)) {
			if (!m_grid[m_now_grid].m_array[getPos(x4, y4)].m_wall_bottom) {
				w2 = w3 = false;
			}
		}
	}

	int k = 0;
	FSFloat u1 = 0, u2 = 0, u3 = 0, u4 = 0;
	if ((x1 >= 0) && (y1 >= 0) && (x1 < m_x) && (y1 < m_y) && !w1) {
		u1 = m_grid[m_now_grid].m_array[getPos(x1, y1)].m_uph * (1 - alpha)
				* beta;
		k++;
	}
	if ((x2 >= 0) && (y2 >= 0) && (x2 < m_x) && (y2 < m_y) && !w2) {
		u2 = m_grid[m_now_grid].m_array[getPos(x2, y2)].m_uph * (1 - alpha)
				* (1 - beta);
		k++;
	}
	if ((x3 >= 0) && (y3 >= 0) && (x3 < m_x) && (y3 < m_y) && !w3) {
		u3 = m_grid[m_now_grid].m_array[getPos(x3, y3)].m_uph * alpha
				* (1 - beta);
		k++;
	}
	if ((x4 >= 0) && (y4 >= 0) && (x4 < m_x) && (y4 < m_y) && !w4) {
		u4 = m_grid[m_now_grid].m_array[getPos(x4, y4)].m_uph * alpha * beta;
		k++;
	}
	assert(k == 0);
	FSFloat newu = (u1 + u2 + u3 + u4) / k;

	//For y
	// 1 4
	// 2 3
	x2 = floor(x);
	y2 = floor(y - 0.5);

	x1 = x2;
	x3 = x2 + 1;
	x4 = x3;

	y1 = y2 + 1;
	y3 = y2;
	y4 = y1;

	//alpha for x, beta for y
	alpha = x - x2;
	beta = y - y2 + 0.5;

	w1 = w2 = w3 = w4 = true;
	//will only block by right wall of P2
	if (alpha < 0.5) {
		w1 = w2 = false;
		if ((x2 >= 0) && (y2 >= 0) && (x2 < m_x) && (y2 < m_y)) {
			if (!m_grid[m_now_grid].m_array[getPos(x2, y2)].m_wall_right) {
				w3 = w4 = false;
			}
		}
	} else {
		w3 = w4 = false;
		if ((x2 >= 0) && (y2 >= 0) && (x2 < m_x) && (y2 < m_y)) {
			if (!m_grid[m_now_grid].m_array[getPos(x2, y2)].m_wall_right) {
				w1 = w2 = false;
			}
		}
	}

	k = 0;
	FSFloat v1 = 0, v2 = 0, v3 = 0, v4 = 0;
	if ((x1 >= 0) && (y1 >= 0) && (x1 < m_x) && (y1 < m_y) && !w1) {
		v1 = m_grid[m_now_grid].m_array[getPos(x1, y1)].m_vph * (1 - alpha)
				* beta;
		k++;
	}
	if ((x2 >= 0) && (y2 >= 0) && (x2 < m_x) && (y2 < m_y) && !w2) {
		v2 = m_grid[m_now_grid].m_array[getPos(x2, y2)].m_vph * (1 - alpha)
				* (1 - beta);
		k++;
	}
	if ((x3 >= 0) && (y3 >= 0) && (x3 < m_x) && (y3 < m_y) && !w3) {
		v3 = m_grid[m_now_grid].m_array[getPos(x3, y3)].m_vph * alpha
				* (1 - beta);
		k++;
	}
	if ((x4 >= 0) && (y4 >= 0) && (x4 < m_x) && (y4 < m_y) && !w4) {
		v4 = m_grid[m_now_grid].m_array[getPos(x4, y4)].m_vph * alpha * beta;
		k++;
	}
	assert(k == 0);
	FSFloat newv = (v1 + v2 + v3 + v4) / k;

	u = newu;
	v = newv;
}

void Simulation::Poisson() {
	//use u* to solve poisson
	//solve Ap=b
	Eigen::VectorXd p, b;
	FSszie total_size = m_x * m_y;
	b.resize(total_size);

	int _new_grid = 1 - m_now_grid;

	for (FSszie i = 0; i < m_x; i++)
		for (FSszie j = 0; j < m_y; j++) {
			FSszie myPos = getPos(i, j);
			b[myPos] = 0;
			if (m_grid[m_now_grid].m_array[myPos].m_fixP) {
				continue; //Dont want to push the rest 4 spaces away anymore.
			}
			//   4   | y
			// 1 p 3 | ^
			//   2   | 0 >x
			FSszie x1, x2, x3, x4, y1, y2, y3, y4;
			x1 = i - 1;
			y1 = j;
			x2 = i;
			y2 = j - 1;
			x3 = i + i;
			y3 = j;
			x4 = i;
			y4 = j + 1;

			if ((x1 >= 0) && (y1 >= 0) && (x1 < m_x) && (y1 < m_y)) {
				FSszie Pos1 = getPos(x1, y1);
				if (!m_grid[m_now_grid].m_array[Pos1].m_wall_right) {
					b[myPos] -= m_grid[_new_grid].m_array[Pos1].m_uph;		//u*
					//subtract the pressure on the right side of equation if it is fixed
					if (m_grid[m_now_grid].m_array[Pos1].m_fixP) {
						b[myPos] -= m_grid[m_now_grid].m_array[Pos1].m_p;
					}
				}
			}
			if ((x2 >= 0) && (y2 >= 0) && (x2 < m_x) && (y2 < m_y)) {
				FSszie Pos2 = getPos(x2, y2);
				//I dont need Pos2's u or v, just for p if it is fixed
				if (!m_grid[m_now_grid].m_array[myPos].m_wall_bottom) {
					b[myPos] += m_grid[_new_grid].m_array[myPos].m_vph;		//u*
					if (m_grid[m_now_grid].m_array[Pos2].m_fixP) {
						b[myPos] -= m_grid[m_now_grid].m_array[Pos2].m_p;
					}
				}
			}
			if ((x3 >= 0) && (y3 >= 0) && (x3 < m_x) && (y3 < m_y)) {
				FSszie Pos3 = getPos(x3, y3);
				//I dont need Pos3's u or v, just for p if it is fixed
				if (!m_grid[m_now_grid].m_array[myPos].m_wall_right) {
					b[myPos] += m_grid[_new_grid].m_array[myPos].m_uph;		//u*
					if (m_grid[m_now_grid].m_array[Pos3].m_fixP) {
						b[myPos] -= m_grid[m_now_grid].m_array[Pos3].m_p;
					}
				}
			}
			if ((x4 >= 0) && (y4 >= 0) && (x4 < m_x) && (y4 < m_y)) {
				FSszie Pos4 = getPos(x4, y4);
				if (!m_grid[m_now_grid].m_array[Pos4].m_wall_bottom) {
					b[myPos] -= m_grid[_new_grid].m_array[Pos4].m_vph;		//u*
					//subtract the pressure on the right side of equation if it is fixed
					if (m_grid[m_now_grid].m_array[Pos4].m_fixP) {
						b[myPos] -= m_grid[m_now_grid].m_array[Pos4].m_p;
					}
				}
			}
		}

	//devided by delta_t
	b = b / m_delta_t;

	p = m_solver.solve(b);
	for (FSszie i = 0; i < m_x; i++)
		for (FSszie j = 0; j < m_y; j++) {
			FSszie myPos = getPos(i, j);
			if (!m_grid[m_now_grid].m_array[myPos].m_fixP) {
				m_grid[m_now_grid].m_array[myPos].m_p = p[myPos];
			}
		}
}

void Simulation::UpdateU() {
	int _new_grid = 1 - m_now_grid;

	for (FSszie i = 0; i < m_x; i++)
		for (FSszie j = 0; j < m_y; j++) {
			FSszie myPos = getPos(i, j);
			//for u
			//if there is a wall the m_fixUPH must be true, so no need to check it
			if (!m_grid[m_now_grid].m_array[myPos].m_fixUPH) {
				if (i + 1 < m_x) {
					FSFloat nabla_pu = m_grid[m_now_grid].m_array[getPos(i + 1,
							j)].m_p - m_grid[m_now_grid].m_array[myPos].m_p;
					m_grid[m_now_grid].m_array[myPos].m_uph =
							m_grid[_new_grid].m_array[myPos].m_uph
									- nabla_pu * m_delta_t;
				}
			}

			//for v
			if (!m_grid[m_now_grid].m_array[myPos].m_fixVPH) {
				if (j + 1 < m_y) {
					FSFloat nabla_pv = m_grid[m_now_grid].m_array[getPos(i,
							j+1)].m_p - m_grid[m_now_grid].m_array[myPos].m_p;
					m_grid[m_now_grid].m_array[myPos].m_vph =
							m_grid[_new_grid].m_array[myPos].m_vph
									- nabla_pv * m_delta_t;
				}
			}
		}
}

void Simulation::Show() {

}

void Simulation::InitPoissonMatrix() {
	FSszie total_size = m_x * m_y;
	m_A.resize(total_size, total_size);
	for (FSszie i = 0; i < m_x; i++)
		for (FSszie j = 0; j < m_y; j++) {
			FSszie myPos = getPos(i, j);
			int k = 0;
			//   4   | y
			// 1 p 3 | ^
			//   2   | 0 >x
			FSszie x1, x2, x3, x4, y1, y2, y3, y4;
			x1 = i - 1;
			y1 = j;
			x2 = i;
			y2 = j - 1;
			x3 = i + i;
			y3 = j;
			x4 = i;
			y4 = j + 1;

			if ((x1 >= 0) && (y1 >= 0) && (x1 < m_x) && (y1 < m_y)) {
				FSszie Pos1 = getPos(x1, y1);
				if (!m_grid[m_now_grid].m_array[Pos1].m_wall_right) {
					if (!m_grid[m_now_grid].m_array[Pos1].m_fixP) {
						m_A.insert(myPos, Pos1) = 1;
					}
					k++;
				}
			}
			if ((x2 >= 0) && (y2 >= 0) && (x2 < m_x) && (y2 < m_y)) {
				FSszie Pos2 = getPos(x2, y2);
				if (!m_grid[m_now_grid].m_array[myPos].m_wall_bottom) {
					if (!m_grid[m_now_grid].m_array[Pos2].m_fixP) {
						m_A.insert(myPos, Pos2) = 1;
					}
					k++;
				}
			}
			if ((x3 >= 0) && (y3 >= 0) && (x3 < m_x) && (y3 < m_y)) {
				FSszie Pos3 = getPos(x3, y3);
				if (!m_grid[m_now_grid].m_array[myPos].m_wall_right) {
					if (!m_grid[m_now_grid].m_array[Pos3].m_fixP) {
						m_A.insert(myPos, Pos3) = 1;
					}
					k++;
				}
			}
			if ((x4 >= 0) && (y4 >= 0) && (x4 < m_x) && (y4 < m_y)) {
				FSszie Pos4 = getPos(x4, y4);
				if (!m_grid[m_now_grid].m_array[Pos4].m_wall_bottom) {
					if (!m_grid[m_now_grid].m_array[Pos4].m_fixP) {
						m_A.insert(myPos, Pos4) = 1;
					}
					k++;
				}
			}
			m_A.insert(myPos, myPos) = -k;
		}

	m_solver.compute(m_A);
	if (m_solver.info() != Eigen::Success) {
		// decomposition failed
		std::cout << "decomposition failed" << std::endl;
		assert(false);
	}

}

void Simulation::InitGL() {
	glutInit(&m_argc, m_argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	if (m_Magnify) {
		glutInitWindowSize(4 * m_width, 4 * m_height);
	} else {
		glutInitWindowSize(m_width, m_height);
	}
	glutCreateWindow("CHL Fluid (C) 2017");
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glDrawBuffer(GL_FRONT);
}

void Simulation::SetP(FSszie x, FSszie y, FSFloat p) {
	m_grid[m_now_grid].m_array[getPos(x, y)].m_p = p;
	m_grid[m_now_grid].m_array[getPos(x, y)].m_fixP = true;
}

void Simulation::SetUPH(FSszie x, FSszie y, FSFloat uph) {
	m_grid[m_now_grid].m_array[getPos(x, y)].m_uph = uph;
	m_grid[m_now_grid].m_array[getPos(x, y)].m_fixUPH = true;
}

void Simulation::SetVPH(FSszie x, FSszie y, FSFloat vph) {
	m_grid[m_now_grid].m_array[getPos(x, y)].m_vph = vph;
	m_grid[m_now_grid].m_array[getPos(x, y)].m_fixVPH = true;
}

void Simulation::SetWall(FSszie x, FSszie y, bool set, bool rightHandSide) {
	if (rightHandSide) {
		m_grid[m_now_grid].m_array[getPos(x, y)].m_wall_right = set;
	} else {
		m_grid[m_now_grid].m_array[getPos(x, y)].m_wall_bottom = set;
	}
}

void Simulation::ChangeDeltaT(FSFloat dt) {
	m_delta_t = dt;
}

void Simulation::ChangeGrid(FSszie x, FSszie y) {
	m_x = x;
	m_y = y;
	m_grid[0] = Grid(m_x, m_y);
	m_grid[1] = Grid(m_x, m_y);
}

void Simulation::ChangeDuration(FSFloat t) {
	m_duration = t;
}

void Simulation::SetMagnify() {
	m_Magnify = true;
}

FSszie Simulation::getPos(FSszie x, FSszie y) {
	return y * m_width + x;
}

FSszie Simulation::getMegaPos(FSszie x, FSszie y) {
	return y * 4 * m_width + x;
}
