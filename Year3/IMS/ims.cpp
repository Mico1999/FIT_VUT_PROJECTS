//
// Created by Matej Jurik on 7. 12. 2021.
//

#include "src/ArgParser/Argopt.h"
#include "src/ArgParser/ArgParser.h"
#include "src/GreenDealSimulation/GreenDealSimulation.h"

/**
 * @brief Application entrypoint
 * @param argc
 * @param argv
 * @return Exit status code
 */
int main(int argc, char **argv)
{
	ArgParser argParser;
	ArgOptions argopt = argParser.parse(argc, argv);

	GreenDealSimulation simulation(argopt);
	return simulation.run();
}

