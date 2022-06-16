//
// Created by Matej Jurik on 8. 12. 2021.
//

#ifndef GREENDEAL_ARGPARSER_H
#define GREENDEAL_ARGPARSER_H

#include <iostream>
#include "Argopt.h"

#define ERROR_ARGUMENT_VALUE_MISSING 2
#define ERROR_ARGUMENT_REQUIRED 3
#define ERROR_ARGUMENT_VALUE_INVALID 4

/**
 * @brief Handles program arguments parsing
 */
class ArgParser
{
private:
	ArgOptions argopts;

	/**
	 * @brief Parse process - get all relevant program arguments, ignore irrelevant
	 */
	void getArguments(int argc, char **argv);

	/**
	 * @brief Validate relevant arguments' values
	 */
	void validateArguments();

	/**
	 * @brief Parse experiment selection argument value
	 * @param optarg String argument value of -x option
	 */
	void parseExperimentSelection(const char *_optarg);
public:
	/**
	 * @brief ArgParser constructor
	 */
	ArgParser();

	/**
	 * @brief Parse program arguments
	 * @return Parsed arguments structure with default, or overwritten data provided by user
	 */
	ArgOptions parse(int argc, char** argv);


	/**
	 * @brief Display usage info
	 */
	static void helpMessage();
};


#endif //GREENDEAL_ARGPARSER_H
