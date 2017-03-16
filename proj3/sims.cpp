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

#define MULTIFICATION 30

Simulation::Simulation(int argc, char** argv) :
		m_argc(argc), m_argv(argv), m_delta_t(0.01), m_duration(0), m_x(320), m_y(
				240), m_width(320), m_height(240), m_Magnify(false), m_now_grid(
				0) {
	m_pixel_data = new MGLpixel[m_width * m_height];
	m_mag_pixel_data = new MGLpixel[MULTIFICATION * m_width * MULTIFICATION
			* m_height];
}

Simulation::~Simulation() {
	delete[] m_pixel_data;
	delete[] m_mag_pixel_data;
}

void Simulation::Display() {
	if (m_Magnify) {
		glViewport(0, 0, MULTIFICATION * m_width, MULTIFICATION * m_height);
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
				for (FSszie y = j * MULTIFICATION; y < (j + 1) * MULTIFICATION;
						++y)
					for (FSszie x = i * MULTIFICATION;
							x < (i + 1) * MULTIFICATION; ++x) {
						m_mag_pixel_data[getMegaPos(x, y)] =
								m_pixel_data[getPixelPos(i, j)];
					}
			}
		glDrawPixels(MULTIFICATION * m_width, MULTIFICATION * m_height, GL_RGBA,
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
	std::cout << "InitGL();" << std::endl;
	InitGL();
	std::cout << "InitPoissonMatrix();" << std::endl;
	InitPoissonMatrix();

	bool a = false;
	std::cout << "MainLoop" << std::endl;
	while (1) {
		if (a) {
			std::cout << "===Tack" << std::endl;
		} else {
			std::cout << "Tick===" << std::endl;
		}
		a = !a;
		ShowDebugGrid();
		std::cin.ignore();
		Tick();
	}

}

void Simulation::ShowDebugGrid() {
	std::cout << "p:" << std::endl;
	for (FSszie j = m_y; j > 0; j--) {
		std::cout.precision(5);
		std::cout << std::fixed;
		for (FSszie i = 0; i < m_x; ++i) {
			std::cout << m_grid[m_now_grid].m_array[getPos(i, j - 1)].m_p
					<< ",";
		}
		std::cout << std::endl;
	}
	std::cout << "u:" << std::endl;
	for (FSszie j = m_y; j > 0; j--) {
		std::cout.precision(5);
		std::cout << std::fixed;
		for (FSszie i = 0; i < m_x; ++i) {
			std::cout << m_grid[m_now_grid].m_array[getPos(i, j - 1)].m_uph
					<< ",";
		}
		std::cout << std::endl;
	}
	std::cout << "v:" << std::endl;
	for (FSszie j = m_y; j > 0; j--) {
		std::cout.precision(5);
		std::cout << std::fixed;
		for (FSszie i = 0; i < m_x; ++i) {
			std::cout << m_grid[m_now_grid].m_array[getPos(i, j - 1)].m_vnh
					<< ",";
		}
		std::cout << std::endl;
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
			if (!m_grid[m_now_grid].m_array[getPos(i, j)].m_fixVNH) {
				FSFloat ug, vg;
				Interpolate(i, j - 0.5, ug, vg);
				FSFloat px = i - m_delta_t * ug;
				FSFloat py = j - 0.5 - m_delta_t * vg;
				FSFloat up, vp;
				Interpolate(px, py, up, vp);
				m_grid[_new_grid].m_array[getPos(i, j)].m_vnh = vp;
			}

		}
	}
}

void Simulation::Interpolate(FSFloat x, FSFloat y, FSFloat& u, FSFloat& v) {

	//For u
	// 1|4
	//--+--
	// 2|3

	int32_t x1, x2, x3, x4, y1, y2, y3, y4;
	x2 = floor(x - 0.5);
	y2 = floor(y);

	x1 = x2;
	x3 = x2 + 1;
	x4 = x3;

	y1 = y2 + 1;
	y3 = y2;
	y4 = y1;

	//alpha for x, beta for y
	FSFloat alpha = x - (x2 + 0.5);
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
//	std::cout << k << "X" << x << "," << y << ";" << x1 << "," << y1 << ";"
//			<< x2 << "," << y2 << ";" << x3 << "," << y3 << ";" << x4 << ","
//			<< y4 << ";" << std::endl;
	assert(k != 0);
	FSFloat newu = (u1 + u2 + u3 + u4) / k;

	//For v
	// 1 4
	// 2 3
	x2 = floor(x);
	y2 = floor(y + 0.5);

	x1 = x2;
	x3 = x2 + 1;
	x4 = x3;

	y1 = y2 + 1;
	y3 = y2;
	y4 = y1;

	//alpha for x, beta for y
	alpha = x - x2;
	beta = y - (y2 - 0.5);

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
		v1 = m_grid[m_now_grid].m_array[getPos(x1, y1)].m_vnh * (1 - alpha)
				* beta;
		k++;
	}
	if ((x2 >= 0) && (y2 >= 0) && (x2 < m_x) && (y2 < m_y) && !w2) {
		v2 = m_grid[m_now_grid].m_array[getPos(x2, y2)].m_vnh * (1 - alpha)
				* (1 - beta);
		k++;
	}
	if ((x3 >= 0) && (y3 >= 0) && (x3 < m_x) && (y3 < m_y) && !w3) {
		v3 = m_grid[m_now_grid].m_array[getPos(x3, y3)].m_vnh * alpha
				* (1 - beta);
		k++;
	}
	if ((x4 >= 0) && (y4 >= 0) && (x4 < m_x) && (y4 < m_y) && !w4) {
		v4 = m_grid[m_now_grid].m_array[getPos(x4, y4)].m_vnh * alpha * beta;
		k++;
	}
//	std::cout << k << "Y" << x << "," << y << ";" << x1 << "," << y1 << ";"
//			<< x2 << "," << y2 << ";" << x3 << "," << y3 << ";" << x4 << ","
//			<< y4 << ";" << std::endl;

	assert(k != 0);
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
				b[myPos] = m_grid[m_now_grid].m_array[myPos].m_p;
				continue; //Dont want to push the rest 4 spaces away anymore.
			}
			//   4   | y
			// 1 p 3 | ^
			//   2   | 0 >x
			FSFloat rightP = 0;
			int32_t x1, x2, x3, x4, y1, y2, y3, y4;
			x1 = i - 1;
			y1 = j;
			x2 = i;
			y2 = j - 1;
			x3 = i + 1;
			y3 = j;
			x4 = i;
			y4 = j + 1;

			if ((x1 >= 0) && (y1 >= 0) && (x1 < m_x) && (y1 < m_y)) {
				FSszie Pos1 = getPos(x1, y1);
				if (!m_grid[m_now_grid].m_array[Pos1].m_wall_right) {
					b[myPos] -= m_grid[_new_grid].m_array[Pos1].m_uph;		//u*
					//subtract the pressure on the right side of equation if it is fixed
					if (m_grid[m_now_grid].m_array[Pos1].m_fixP) {
						rightP += m_grid[m_now_grid].m_array[Pos1].m_p;
					}
				}
			}
			if ((x2 >= 0) && (y2 >= 0) && (x2 < m_x) && (y2 < m_y)) {
				FSszie Pos2 = getPos(x2, y2);
				//I dont need Pos2's u or v, just for p if it is fixed
				if (!m_grid[m_now_grid].m_array[myPos].m_wall_bottom) {
					b[myPos] += m_grid[_new_grid].m_array[myPos].m_vnh;		//u*
					if (m_grid[m_now_grid].m_array[Pos2].m_fixP) {
						rightP += m_grid[m_now_grid].m_array[Pos2].m_p;
					}
				}
			}
			if ((x3 >= 0) && (y3 >= 0) && (x3 < m_x) && (y3 < m_y)) {
				FSszie Pos3 = getPos(x3, y3);
				//I dont need Pos3's u or v, just for p if it is fixed
				if (!m_grid[m_now_grid].m_array[myPos].m_wall_right) {
					b[myPos] += m_grid[_new_grid].m_array[myPos].m_uph;		//u*
					if (m_grid[m_now_grid].m_array[Pos3].m_fixP) {
						rightP += m_grid[m_now_grid].m_array[Pos3].m_p;
					}
				}
			}
			if ((x4 >= 0) && (y4 >= 0) && (x4 < m_x) && (y4 < m_y)) {
				FSszie Pos4 = getPos(x4, y4);
				if (!m_grid[m_now_grid].m_array[Pos4].m_wall_bottom) {
					b[myPos] -= m_grid[_new_grid].m_array[Pos4].m_vnh;		//u*
					//subtract the pressure on the right side of equation if it is fixed
					if (m_grid[m_now_grid].m_array[Pos4].m_fixP) {
						rightP += m_grid[m_now_grid].m_array[Pos4].m_p;
					}
				}
			}
			std::cout << "b[myPos]:" << std::endl;
			std::cout << b[myPos] << std::endl;
			b[myPos] = b[myPos] / m_delta_t - rightP;
		}

	std::cout << "b:" << std::endl;
	std::cout << b << std::endl;
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
			if (!m_grid[m_now_grid].m_array[myPos].m_fixVNH) {
				if (j >= 1) {
					FSFloat nabla_pv = m_grid[m_now_grid].m_array[getPos(i,
							j - 1)].m_p - m_grid[m_now_grid].m_array[myPos].m_p;
					m_grid[m_now_grid].m_array[myPos].m_vnh =
							m_grid[_new_grid].m_array[myPos].m_vnh
									- nabla_pv * m_delta_t;
				}
			}
		}
}

void Simulation::Show() {
	for (FSszie i = 0; i < m_width; ++i) {
		for (FSszie j = 0; j < m_height; ++j) {
			m_pixel_data[getPixelPos(i, j)] = Make_Pixel(0, 51, 102);
		}
	}

	size_t q_size = m_parts.size();
	for (size_t i = 0; i < q_size; ++i) {
		FSFloat u, v;
		FSFloat x = m_parts.front().x;
		FSFloat y = m_parts.front().y;
		std::cout<<"Old: "<<x<<","<<y<<";";
		m_parts.pop_front();
		Interpolate(x, y, u, v);
		x = x + u * m_delta_t;
		y = y - v * m_delta_t;
		std::cout<<"UV: "<<u<<","<<v<<";";
		std::cout<<"New: "<<x<<","<<y<<";"<<std::endl;
		if ((x > -0.5) && (y > -0.5) && (x < m_width - 0.5)
				&& (y < m_height - 0.5)) {

			m_pixel_data[getPixelPos(round(x), round(y))] = Make_Pixel(255, 255,
					255);

			m_parts.push_back(Partical(x, y));
		}
	}

	if (q_size == 0) {
		int n = 2;
		FSFloat oneovernp1 = 1.0 / (n + 1);
		for (int i = 1; i <= n; ++i) {
			for (int j = 1; j <= n; ++j) {
				m_parts.push_back(
						Partical(m_width - 1.5 + i * oneovernp1,
								m_height - 1.5 + j * oneovernp1));
			}
		}
	}

	Display();
}

void Simulation::InitPoissonMatrix() {
	FSszie total_size = m_x * m_y;
//	std::cout<<"m_A.nonZeros():"<<m_A.nonZeros()<<std::endl;
	m_A.resize(total_size, total_size);
//	std::cout<<"m_A.nonZeros():"<<m_A.nonZeros()<<std::endl;
//	m_A.setZero();
//	std::cout<<"m_A.nonZeros():"<<m_A.nonZeros()<<std::endl;
	for (FSszie i = 0; i < m_x; i++)
		for (FSszie j = 0; j < m_y; j++) {
			FSszie myPos = getPos(i, j);
			if (m_grid[m_now_grid].m_array[myPos].m_fixP) {
				m_A.insert(myPos, myPos) = 1;
				continue;
			}
			std::cout << "myPos: " << i << "," << j << std::endl;
			int k = 0;
			//   4   | y
			// 1 p 3 | ^
			//   2   | 0 >x
			int32_t x1, x2, x3, x4, y1, y2, y3, y4;
			x1 = i - 1;
			y1 = j;
			x2 = i;
			y2 = j - 1;
			x3 = i + 1;
			y3 = j;
			x4 = i;
			y4 = j + 1;

			if ((x1 >= 0) && (y1 >= 0) && (x1 < m_x) && (y1 < m_y)) {
				FSszie Pos1 = getPos(x1, y1);
				if (!m_grid[m_now_grid].m_array[Pos1].m_wall_right) {
					if (!m_grid[m_now_grid].m_array[Pos1].m_fixP) {
						std::cout << "m_A.insert(myPos, Pos1) = 1;"
								<< std::endl;
						m_A.insert(myPos, Pos1) = 1;
					}
					k++;
				}
			}
			if ((x2 >= 0) && (y2 >= 0) && (x2 < m_x) && (y2 < m_y)) {
				FSszie Pos2 = getPos(x2, y2);
				if (!m_grid[m_now_grid].m_array[myPos].m_wall_bottom) {
					if (!m_grid[m_now_grid].m_array[Pos2].m_fixP) {
						std::cout << "m_A.insert(myPos, Pos2) = 1;"
								<< std::endl;
						m_A.insert(myPos, Pos2) = 1;
					}
					k++;
				}
			}
			if ((x3 >= 0) && (y3 >= 0) && (x3 < m_x) && (y3 < m_y)) {
				FSszie Pos3 = getPos(x3, y3);
				if (!m_grid[m_now_grid].m_array[myPos].m_wall_right) {
					if (!m_grid[m_now_grid].m_array[Pos3].m_fixP) {
						std::cout << "m_A.insert(myPos, Pos3) = 1;"
								<< std::endl;
//						std::cout<<"Pos3:"<<Pos3<<std::endl;
						m_A.insert(myPos, Pos3) = 1;
					}
					k++;
				}
			}
			if ((x4 >= 0) && (y4 >= 0) && (x4 < m_x) && (y4 < m_y)) {
				FSszie Pos4 = getPos(x4, y4);
				if (!m_grid[m_now_grid].m_array[Pos4].m_wall_bottom) {
					if (!m_grid[m_now_grid].m_array[Pos4].m_fixP) {
						std::cout << "m_A.insert(myPos, Pos4) = 1;"
								<< std::endl;
//						std::cout<<"Pos4:"<<Pos4<<","<<x4<<","<<y4<<std::endl;
						m_A.insert(myPos, Pos4) = 1;
					}
					k++;
				}
			}
//			std::cout<<"m_A.insert(myPos, myPos) = -k;"<<std::endl;
			m_A.insert(myPos, myPos) = -k;
		}
	std::cout << "A:" << std::endl;
	std::cout << m_A << std::endl;
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
		glutInitWindowSize(MULTIFICATION * m_width, MULTIFICATION * m_height);
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

void Simulation::SetVNH(FSszie x, FSszie y, FSFloat vph) {
	m_grid[m_now_grid].m_array[getPos(x, y)].m_vnh = vph;
	m_grid[m_now_grid].m_array[getPos(x, y)].m_fixVNH = true;
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
	m_width = x;
	m_height = y;
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
	return y * m_x + x;
}

FSszie Simulation::getPixelPos(FSszie x, FSszie y) {
	return y * m_width + x;
}

FSszie Simulation::getMegaPos(FSszie x, FSszie y) {
	return y * MULTIFICATION * m_width + x;
}
