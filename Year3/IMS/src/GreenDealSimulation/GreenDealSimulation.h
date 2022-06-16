//
// Created by Matej Jurik on 8. 12. 2021.
//

#ifndef GREENDEAL_GREENDEALSIMULATION_H
#define GREENDEAL_GREENDEALSIMULATION_H

#include <iostream>
#include <cmath>
#include <cstdarg>
#include "../ArgParser/Argopt.h"

#define ENERGY_LOSS_THRESHOLD 1 // 1 GWh reserve for computation deviation

/** @brief Wrapper for simulation running check */
#define SHOULD_RUN_EXPERIMENT(n) \
	(this->simParams.experimentSelection == (n) || this->simParams.experimentSelection == 0)

/** @brief Print current simulation function name */
#define _OUTPUT_SIMULATION_HEADER() std::cout << "##### " << __FUNCTION__ << " #####" << std::endl

/**
 * @brief Print simulation parameters
 * @note Only serves as syntactic sugar - adds empty string as the required last variadic argument
 */
#define _OUTPUT_SIMULATION_PARAMS(paramInfo, ...) this->_output_simulation_params( \
	(paramInfo), ##__VA_ARGS__, "" \
)

/**
 * @brief Handles simulating aspects of the green deal agreement to reduce
 * 		  establishments' CO2 footprint by year 2030
 */
class GreenDealSimulation
{
private:
	ArgOptions simParams;


	/**
	 * EXPERIMENT NUMBER 1
	 *
	 * @brief Simulate yearly energy production & consumption during the relevant years
	 * 		  Check whether the country will remain energy-independent or not
	 */
	void impact_of_coal_energy_reduction_on_CR_energy_independence();

	/**
	 * EXPERIMENT NUMBER 2
	 *
	 * @brief Simulate yearly energy production & consumption during the relevant years
	 * 		  with the additional hypothesis, that these years' energy consumption will be
	 * 		  5% lower due to COVID pandemic world-wide situation
	 * 		  Check whether the country will remain energy-independent or not
	 */
	void impact_of_coal_energy_reduction_on_CR_energy_independence_during_covid();

	/**
	 * @brief Do the yearly energy production,balance & country independence calculations based on
	 * 		  the provided yearly energy consumption factor and output results
	 */
	void compute_impact_of_coal_energy_reduction_on_CR_energy_independence(double yearly_energy_consumption) const;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	/**
	 * EXPERIMENT NUMBER 3
	 *
	 * @brief Simulate gradually replacing brown coal energy for natural gas energy and
	 * 		  see whether this helps with the country staying energy independent
	 */
	void impact_of_natural_gas_energy_increase_on_CR_energy_independence();

	/**
	 * @brief Do the yearly energy production, balance & country independence calculations
	 * 		  based on average yearly energy consumption factor while accounting for gradual
	 * 		  brown coal replacement for natural gas and output results
	 */
	void compute_impact_of_natural_gas_energy_increase_on_CR_energy_independence() const;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /**
     * EXPERIMENT NUMBER 4
     *
	 * @brief Simulate gradually decreasing brown coal energy and calculate amount of
     *        renewable energy needed to save CR energy independence
	 */
    void required_renewable_energy_increase_to_save_CR_energy_independence();

    /**
     * @brief Do the yearly energy production, balance & country independence calculations
     * 		  based on average yearly energy consumption factor while accounting for gradual
     * 		  brown coal decreasing and calculate required increase of amount
     * 		  of renewable energy to save CR energy independence
     */
    void compute_required_renewable_energy_increase_to_save_CR_energy_independence(double yearly_energy_consumption) const;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	/**
     * EXPERIMENT NUMBER 5
     *
	 * @brief Simulate gradual energy production loss by coal energy production reduction over the
	 * 		  relevant years. If energy independence is lost in the simulation, calculate the maximum
	 * 		  yearly coal energy production reduction (in Megatons for brown and black coal) feasible
	 * 		  to preserve energy independence
	 */
	void maximum_possible_decarbonization_to_preserve_CR_energy_independence();

	/**
	 * @brief Do the necessary calculations in order to determine whether the energy independence will be lost
	 * 		  using the default yearly coal energy production reduction. In case it will, calculate how much the coal
	 * 		  energy production can be reduced yearly while staying energy independent till the end of the researched
	 * 		  year span
	 */
	void compute_maximum_possible_decarbonization_that_wont_affect_CR_energy_independence() const;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	/**
	 * @brief Text formatting helper - print simulation used parameters info
	 * @pre Variadic arguments must be terminated with empty string (last argument MUST be empty string)
	 *
	 * Does nothing in graphical mode
	 */
	void _output_simulation_params(const std::string& paramInfo, ...) const;

	/**
	 * @brief Text formatting helper - separates group results within the same simulation
	 *
	 * Does nothing in graphical mode
	 */
	void _output_delimit_result_group() const;

	/**
	 * @brief Text formatting helper - separates simulation additional results
	 *
	 * Does nothing in graphical mode
	 */
	void _output_simulation_additional_results() const;

	/**
	 * @brief Text formatting helper - separates entire simulations
	 *
	 * Does nothing in graphical mode
	 */
	void _output_delimit_simulations() const;
public:
	/**
	 * @brief GreenDealSimulation constructor - determines which simulations to run according
	 * 		  to 'argopt' parameter
	 * @param argopt
	 */
	explicit GreenDealSimulation(ArgOptions argopt);

	/**
	 * @brief Run all simulations
	 * 		  Functions as a program terminating function
	 * @return Program exit code
	 */
	int run();
};


#endif //GREENDEAL_GREENDEALSIMULATION_H
