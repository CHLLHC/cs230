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

	while (1) {
		int opt = getopt(argc, argv, "i:");
		if (opt == -1)
			break;
		switch (opt) {
		case 'i':
			input_file = optarg;
			break;
		}
	}
	if (!input_file)
		Usage(argv[0]);

	Simulation sims;

	Parse(sims, input_file);

	sims.Run();

	return 0;
}

