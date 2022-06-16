//
// Created by Matej Jurik on 8. 12. 2021.
//

#include "ArgParser.h"


ArgParser::ArgParser()
{
}

ArgOptions ArgParser::parse(int argc, char** argv)
{
	this->getArguments(argc, argv);
	this->validateArguments();

	return this->argopts;
}

void ArgParser::getArguments(int argc, char **argv)
{
	// Suppress any argument parsing errors
	// opterr = 0;

	// Get all valid arguments
	while (true)
	{
//		switch(getopt(argc, argv, "hHx:g"))
		switch(getopt(argc, argv, "hHx:"))
		{
			// Display program help
			case 'h':
			case 'H':
				ArgParser::helpMessage();
				break;

			// Experiment selection
			case 'x':
				this->parseExperimentSelection(optarg);
				break;

			// Graphical output flag
//			case 'g':
//				this->argopts.graphicalOutput = true;
//				break;

			// Any valid, but not yet handled
			default:
				std::cout << "Parameter " << argv[optind - 1] <<  " is not being handled yet!" << std::endl;
				ArgParser::helpMessage();
				break;

			// Missing required argument
			case '?':
				exit(ERROR_ARGUMENT_VALUE_MISSING);

			// End parsing
			case -1:
				return;
		}
	}
}

void ArgParser::parseExperimentSelection(const char *_optarg)
{
	// Selection number (cast to valid long)
	char *conversionRemainder;
	long experimentNumber = std::strtol(_optarg, &conversionRemainder, 10);
	std::string stringRemainder = std::string(conversionRemainder);

	if (stringRemainder.length() == 0)
	{
		// Catch experiment range errors
		if (experimentNumber < 1 || experimentNumber > EXPERIMENT_TOTAL_COUNT)
		{
			std::cerr << "Experiment selection invalid - choose an experiment in range 1 to "
					  << EXPERIMENT_TOTAL_COUNT << std::endl;
			exit(ERROR_ARGUMENT_VALUE_INVALID);
		}

		// Update selection
		this->argopts.experimentSelection = experimentNumber;
	}
	else
	{
		// Invalid number
		std::cerr
			<< "Experiment selection invalid - please provide valid number"
			<< std::endl;
		exit(ERROR_ARGUMENT_VALUE_INVALID);
	}
}

void ArgParser::validateArguments()
{
	// Validate argument values
}

void ArgParser::helpMessage()
{
//	std::cout << "Usage: ./GreenDeal [-h|-H] [-x experiment_number] [-g]" << std::endl;
	std::cout << "Usage: ./GreenDeal [-h|-H] [-x experiment_number]" << std::endl;
	std::cout << "       -h|-H                  -  display help" << std::endl;
	std::cout << "       -x experiment_number   -  execute specific experiment (see docs for experiment numbers)" << std::endl;

	exit(EXIT_SUCCESS);
}
