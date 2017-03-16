/*
 * main.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: chl
 */

#include <iostream>
#include "sims.h"
#include <png.h>
#include <cassert>

void Usage(const char* exec) {
	std::cerr << "Usage: " << exec << " -i <test-file>" << std::endl;
	exit(1);
}

void Parse(Simulation& sims, const char* test_file);

int main(int argc, char** argv) {

	const char* input_file = 0;
	bool zoom = false, debug = false;

	while (1) {
		int opt = getopt(argc, argv, "i:md");
		if (opt == -1)
			break;
		switch (opt) {
		case 'i':
			input_file = optarg;
			break;
		case 'm':
			zoom = true;
			break;
		case 'd':
			debug = true;
			break;
		}
	}
	if (!input_file)
		Usage(argv[0]);

	Simulation sims(argc, argv);
	if (zoom){
		sims.SetMagnify();
	}
	if (debug){
		sims.SetDebugFlag();
	}

	Parse(sims, input_file);

	sims.Run();

	return 0;
}

