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

Simulation::Simulation(int argc, char** argv) :
		m_argc(argc), m_argv(argv), m_delta_t(0), m_duration(0), m_x(320), m_y(
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
	MGLpixel white = Make_Pixel(255, 255, 255);

	int k = 0;
	while (1) {
		memset(m_pixel_data, 0, sizeof(MGLpixel) * m_width * m_height);
		for (int i = 0; i < 200; ++i) {
			m_pixel_data[getPos(k, i)] = white;
		}
		k = (k + 1) % 320;
		Display();
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

void Simulation::SetU(FSszie x, FSszie y, const FSF2D &u) {
	m_grid[m_now_grid].m_array[getPos(x, y)].m_u = u;
	m_grid[m_now_grid].m_array[getPos(x, y)].m_fixU = true;
}

void Simulation::SetWall(FSszie x, FSszie y, bool set, bool rightHandSide) {
	if (rightHandSide) {
		m_grid[m_now_grid].m_array[getPos(x, y)].m_wall_right = set;
	} else {
		m_grid[m_now_grid].m_array[getPos(x, y)].m_wall_bottom = set;
	}
}

void Show() {

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

