/*
 * parse.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: chl
 */

#include <map>
#include <sstream>
#include <string>
#include <iostream>
#include "sims.h"

void Parse(Simulation& sims, const char* test_file) {
	FILE* F = fopen(test_file, "r");
	if (!F) {
		printf("Failed to open file '%s'\n", test_file);
		exit(EXIT_FAILURE);
	}

	char buff[1000];

	while (fgets(buff, sizeof(buff), F)) {
		std::stringstream ss(buff);
		std::string item, name;
		if (!(ss >> item) || !item.size() || item[0] == '#')
			continue;
		if (item == "size") {
			FSszie w, h;
			ss >> w >> h;
			assert(ss);
			sims.ChangeGrid(w, h);
		} else if (item == "mag") {
			FSFloat m;
			ss >> m;
			assert(ss);
			sims.ChangeMagnitude(m);
		} else if (item == "wmag") {
			FSFloat wm;
			ss >> wm;
			assert(ss);
			sims.ChangeWindowMagnitude(wm);
		} else if (item == "delta") {
			FSFloat dt;
			ss >> dt;
			assert(ss);
			sims.ChangeDeltaT(dt);
		} else if (item == "cellp") {
			FSszie x, y;
			FSFloat p;
			ss >> x >> y >> p;
			assert(ss);
			sims.SetP(x, y, p);
		} else if (item == "celluph") {
			FSszie x, y;
			FSFloat uph;
			ss >> x >> y >> uph;
			assert(ss);
			sims.SetUPH(x, y, uph);
		} else if (item == "cellvnh") {
			FSszie x, y;
			FSFloat vnh;
			ss >> x >> y >> vnh;
			assert(ss);
			sims.SetVNH(x, y, vnh);
		} else if (item == "pitv") {
			FSFloat pitv;
			ss >> pitv;
			assert(ss);
			sims.ChangeNewPartsInterval(pitv);
		} else if (item == "pps") {
			FSszie pps;
			ss >> pps;
			assert(ss);
			sims.ChangePartsPerShow(pps);
		} else if (item == "sp") {
			FSszie x, y;
			ss >> x >> y;
			assert(ss);
			sims.SetShowPoint(x, y);
		} else if (item == "wall") {
			FSszie x, y;
			int right;
			ss >> x >> y >> right;
			assert(ss);
			if (right) {
				sims.SetWall(x, y, true, true);
			} else {
				sims.SetWall(x, y, true, false);
			}
		} else {
			std::cout << "Failed to parse: " << buff << std::endl;
			exit(EXIT_FAILURE);
		}
	}

}

