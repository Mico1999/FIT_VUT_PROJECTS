//
// Created by Matej Jurík, Marek Miček on 7. 12. 2021.
//

#ifndef GREENDEAL_ARGOPT_H
#define GREENDEAL_ARGOPT_H


#include <getopt.h>
#include <cstdint>
#include <string>

/**
 * @note Default program behavioral parameters section
 */
#define EXPERIMENT_TOTAL_COUNT 5

#define EXPERIMENT_SELECTION 0  // 0 = Select all experiments

/**
 * @note Default simulation parameters section
 */
#define CO2_2020_FOOTPRINT        85.415  // [Mega ton]
#define CO2_2030_FOOTPRINT_TARGET 69.885  // [Mega ton]

#define CO2_YEARLY_FOOTPRINT_REDUCE             1.27 // [Mega ton]
#define CO2_YEARLY_FOOTPRINT_REDUCE_BROWN_COAL  1.2  // [Mega ton]
//#define CO2_YEARLY_FOOTPRINT_REDUCE_BROWN_COAL 0.375782  // Max possible decarbonization rate [Mega ton]
#define CO2_YEARLY_FOOTPRINT_REDUCE_BLACK_COAL  0.07 // [Mega ton]
//#define CO2_YEARLY_FOOTPRINT_REDUCE_BLACK_COAL  0.0260272  // Max possible decarbonization rate [Mega ton]

#define CO2_YEARLY_FOOTPRINT_REDUCE_BROWN_COAL_RATIO 0.945 // [% of total CO2_YEARLY_FOOTPRINT_REDUCE]
#define CO2_YEARLY_FOOTPRINT_REDUCE_BLACK_COAL_RATIO 0.055 // [% of total CO2_YEARLY_FOOTPRINT_REDUCE]

#define BROWN_COAL_ENERGY_FOOTPRINT_PER_GWH 0.000360   // [Mega ton/GWh]
#define BLACK_COAL_ENERGY_FOOTPRINT_PER_GWH 0.000330   // [Mega ton/GWh]
#define NATURAL_GAS_ENERGY_FOOTPRINT_PER_GWH 0.000200  // [Mega ton/GWh]

#define ENERGY_YEARLY_PRODUCTION_2020_5YEARS_AVERAGE 81443.4  // [GWh]
#define ENERGY_YEARLY_CONSUMPTION 73097.22                    // [GWh]
#define ENERGY_YEARLY_CONSUMPTION_COVID 69438.559             // [GWh]

#define AVERAGE_NUCLEAR_ENERGY_YEARLY_PRODUCTION 28530.92       // [GWh]
#define AVERAGE_NATURAL_GAS_ENERGY_YEARLY_PRODUCTION 4479.94    // [GWh]
#define AVERAGE_BIO_GAS_ENERGY_YEARLY_PRODUCTION 2593.72        // [GWh]
#define AVERAGE_BIO_MASS_ENERGY_YEARLY_PRODUCTION 2259.02       // [GWh]
#define AVERAGE_HYDROPOWER_ENERGY_YEARLY_PRODUCTION 1930.22     // [GWh]
#define AVERAGE_OTHER_GASES_ENERGY_YEARLY_PRODUCTION 2643.44    // [GWh]
#define AVERAGE_PUMP_ENERGY_YEARLY_PRODUCTION 1176.48           // [GWh] (translation: Precerpavaci energie)

#define BROWN_COAL_ENERGY_YEARLY_PRODUCTION 29073.6  // [GWh]
#define BROWN_COAL_ENERGY_YEARLY_PRODUCTION_REDUCE \
	 CO2_YEARLY_FOOTPRINT_REDUCE_BROWN_COAL / BROWN_COAL_ENERGY_FOOTPRINT_PER_GWH  // [GWh]

#define BLACK_COAL_ENERGY_YEARLY_PRODUCTION 1914.2   // [GWh]
#define BLACK_COAL_ENERGY_YEARLY_PRODUCTION_REDUCE \
	 CO2_YEARLY_FOOTPRINT_REDUCE_BLACK_COAL / BLACK_COAL_ENERGY_FOOTPRINT_PER_GWH //[GWh]


#define NATURAL_GAS_ENERGY_YEARLY_PRODUCTION                    6586.9 // year 2020 [GWh]
/**
 * @note Expected annual gas production increase percentage
 *       Calculated as the average percentage of annual gas energy increase from year 2016 to 2020
 */
#define NATURAL_GAS_ENERGY_YEARLY_PRODUCTION_INCREASE_PERCENTAGE 0.1987


#define PHOTOVOLTAICS_ENERGY_YEARLY_PRODUCTION          2235.1  // year 2020 [GWh]
#define PHOTOVOLTAICS_ENERGY_YEARLY_PRODUCTION_INCREASE 529.43  // [GWh]

#define WIND_ENERGY_YEARLY_PRODUCTION             699.1    // year 2020 [GWh]
#define WIND_ENERGY_YEARLY_PRODUCTION_INCREASE    100.11   // [GWh]

#define AVERAGE_ENERGY_YEARLY_PRODUCTION     \
	AVERAGE_NUCLEAR_ENERGY_YEARLY_PRODUCTION \
	+ AVERAGE_NATURAL_GAS_ENERGY_YEARLY_PRODUCTION \
	+ AVERAGE_BIO_GAS_ENERGY_YEARLY_PRODUCTION     \
	+ AVERAGE_BIO_MASS_ENERGY_YEARLY_PRODUCTION    \
	+ AVERAGE_HYDROPOWER_ENERGY_YEARLY_PRODUCTION  \
	+ AVERAGE_OTHER_GASES_ENERGY_YEARLY_PRODUCTION \
	+ AVERAGE_PUMP_ENERGY_YEARLY_PRODUCTION

#define AVERAGE_ENERGY_YEARLY_PRODUCTION_WITHOUT_NATURAL_GAS \
	AVERAGE_NUCLEAR_ENERGY_YEARLY_PRODUCTION       \
	+ AVERAGE_BIO_GAS_ENERGY_YEARLY_PRODUCTION     \
	+ AVERAGE_BIO_MASS_ENERGY_YEARLY_PRODUCTION    \
	+ AVERAGE_HYDROPOWER_ENERGY_YEARLY_PRODUCTION  \
	+ AVERAGE_OTHER_GASES_ENERGY_YEARLY_PRODUCTION \
	+ AVERAGE_PUMP_ENERGY_YEARLY_PRODUCTION

#define AVERAGE_RENEWABLE_ENERGY_YEARLY_PRODUCTION \
    AVERAGE_HYDROPOWER_ENERGY_YEARLY_PRODUCTION    \
    + AVERAGE_BIO_GAS_ENERGY_YEARLY_PRODUCTION     \
	+ AVERAGE_BIO_MASS_ENERGY_YEARLY_PRODUCTION

/**
 * @brief Program arguments structure containing default values
 * @note These arguments use default simulation symbolic constants defined in this header
 * 		 and are overridable by user input (@see ArgParser.h)
 */
typedef struct {
	// PROGRAM BEHAVIORAL PARAMETERS
	uint experimentSelection = EXPERIMENT_SELECTION;

	bool graphicalOutput = false;

	// SIMULATION PARAMETERS
	double initialCo2Footprint = CO2_2020_FOOTPRINT;
	double targetCo2Footprint = CO2_2030_FOOTPRINT_TARGET;

	double brownCoalYearlyFootprintReduce = CO2_YEARLY_FOOTPRINT_REDUCE_BROWN_COAL;
	double blackCoalYearlyFootprintReduce = CO2_YEARLY_FOOTPRINT_REDUCE_BLACK_COAL;

	double naturalGasEnergyFootprintPerGwh = NATURAL_GAS_ENERGY_FOOTPRINT_PER_GWH;

	double energyInitialYearlyProduction = ENERGY_YEARLY_PRODUCTION_2020_5YEARS_AVERAGE;
	double energyYearlyConsumption = ENERGY_YEARLY_CONSUMPTION;
	double energyYearlyConsumptionDuringCovid = ENERGY_YEARLY_CONSUMPTION_COVID;

	double energyYearlyNaturalGasProduction = NATURAL_GAS_ENERGY_YEARLY_PRODUCTION;
	double energyYearlyNaturalGasProductionIncreasePercentage =
			NATURAL_GAS_ENERGY_YEARLY_PRODUCTION_INCREASE_PERCENTAGE;

	double energyYearlyPhotovoltaicsProduction = PHOTOVOLTAICS_ENERGY_YEARLY_PRODUCTION;
	double energyYearlyPhotovoltaicsProductionIncrease = PHOTOVOLTAICS_ENERGY_YEARLY_PRODUCTION_INCREASE;

	double energyYearlyWindEnergyProduction = WIND_ENERGY_YEARLY_PRODUCTION;
	double energyYearlyWindEnergyProductionIncrease = WIND_ENERGY_YEARLY_PRODUCTION_INCREASE;

	double energyYearlyBrownCoalEnergyProduction = BROWN_COAL_ENERGY_YEARLY_PRODUCTION;
	double energyYearlyBrownCoalEnergyProductionReduce = BROWN_COAL_ENERGY_YEARLY_PRODUCTION_REDUCE;

	double energyYearlyBlackCoalEnergyProduction = BLACK_COAL_ENERGY_YEARLY_PRODUCTION;
	double energyYearlyBlackCoalEnergyProductionReduce = BLACK_COAL_ENERGY_YEARLY_PRODUCTION_REDUCE;

	double energyAverageYearlyProduction = AVERAGE_ENERGY_YEARLY_PRODUCTION;
	double energyAverageYearlyProductionWithoutNaturalGas = AVERAGE_ENERGY_YEARLY_PRODUCTION_WITHOUT_NATURAL_GAS;
    double renewableEnergyAverageYearlyProduction = AVERAGE_RENEWABLE_ENERGY_YEARLY_PRODUCTION;
} ArgOptions;


#endif //GREENDEAL_ARGOPT_H
