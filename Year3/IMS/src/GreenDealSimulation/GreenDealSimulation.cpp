//
// Created by Matej Jurík, Marek Miček on 8. 12. 2021.
//

#include "GreenDealSimulation.h"

GreenDealSimulation::GreenDealSimulation(ArgOptions argopt) : simParams(argopt)
{
}

int GreenDealSimulation::run()
{

	if (SHOULD_RUN_EXPERIMENT(1))
		this->impact_of_coal_energy_reduction_on_CR_energy_independence();

	if (SHOULD_RUN_EXPERIMENT(2))
		this->impact_of_coal_energy_reduction_on_CR_energy_independence_during_covid();

	if (SHOULD_RUN_EXPERIMENT(3))
		this->impact_of_natural_gas_energy_increase_on_CR_energy_independence();

	if (SHOULD_RUN_EXPERIMENT(4))
		this->required_renewable_energy_increase_to_save_CR_energy_independence();

	if (SHOULD_RUN_EXPERIMENT(5))
		this->maximum_possible_decarbonization_to_preserve_CR_energy_independence();

	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GreenDealSimulation::impact_of_coal_energy_reduction_on_CR_energy_independence()
{
	// Announce which simulation is being run
	_OUTPUT_SIMULATION_HEADER();
	_OUTPUT_SIMULATION_PARAMS("Average yearly energy consumption");
	this->_output_delimit_result_group();

	// Run simulation
	this->compute_impact_of_coal_energy_reduction_on_CR_energy_independence(
			this->simParams.energyYearlyConsumption);

	// Delimit simulations
	this->_output_delimit_simulations();
}

void GreenDealSimulation::impact_of_coal_energy_reduction_on_CR_energy_independence_during_covid()
{
	// Announce which simulation is being run
	_OUTPUT_SIMULATION_HEADER();
	_OUTPUT_SIMULATION_PARAMS("Average yearly energy consumption approximation during covid");
	this->_output_delimit_result_group();

	// Run simulation
	this->compute_impact_of_coal_energy_reduction_on_CR_energy_independence(
			this->simParams.energyYearlyConsumptionDuringCovid);

	// Delimit simulations
	this->_output_delimit_simulations();
}

void GreenDealSimulation::compute_impact_of_coal_energy_reduction_on_CR_energy_independence(double yearly_energy_consumption) const
{
	// Output vars
	double years_energy_production[9];
	int independence_lost_year = -1;

	// Independence status
	bool isIndependent = true;

	// Energy production variable factors
	double photovoltaics_energy_production = this->simParams.energyYearlyPhotovoltaicsProduction;
	double wind_energy_production = this->simParams.energyYearlyWindEnergyProduction;
	double brown_coal_energy_production = this->simParams.energyYearlyBrownCoalEnergyProduction;
	double black_coal_energy_production = this->simParams.energyYearlyBlackCoalEnergyProduction;

	// Yearly simulation
	for (int year = 0; year < 9; year++)
	{
		double annual_total_energy_production = 0;
		// Energy increases
		photovoltaics_energy_production += this->simParams.energyYearlyPhotovoltaicsProductionIncrease;
		wind_energy_production += this->simParams.energyYearlyWindEnergyProductionIncrease;

		// Energy decreases
		brown_coal_energy_production =
				std::max(0.0, brown_coal_energy_production - this->simParams.energyYearlyBrownCoalEnergyProductionReduce);
		black_coal_energy_production =
				std::max(0.0, black_coal_energy_production - this->simParams.energyYearlyBlackCoalEnergyProductionReduce);

		// Compute yearly total for current year
		annual_total_energy_production +=
				this->simParams.energyAverageYearlyProduction
				+ photovoltaics_energy_production
				+ wind_energy_production
				+ brown_coal_energy_production
				+ black_coal_energy_production;

		// Record results
		years_energy_production[year] = annual_total_energy_production;

		std::cout << "Energy production for year 202" << year + 1 << " = " << \
			annual_total_energy_production << " GWh";
		std::cout << std::endl;

		std::cout << "Energy consumption = " << yearly_energy_consumption << " GWh";
		std::cout << std::endl;

		std::cout << "Resulting energy balance = " << \
			annual_total_energy_production - yearly_energy_consumption << " GWh";
		std::cout << std::endl;

		// Determine whether still independent
		double energyLost = yearly_energy_consumption - annual_total_energy_production;
		bool losingIndependence = energyLost > ENERGY_LOSS_THRESHOLD;

		if (losingIndependence && isIndependent) // check for isIndependent to only run statement body once
		{
			isIndependent = false;
			independence_lost_year = year + 1;
		}

		// Delimit respective years
		this->_output_delimit_result_group();
	}


	// Output
	this->_output_simulation_additional_results();
	if (independence_lost_year != -1)
	{
		std::cout << "\tEnergy independence has been lost in year 202" << independence_lost_year;
		std::cout << std::endl;
	}
	else
	{
		std::cout << "\tEnergy independence has persevered" << std::endl;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GreenDealSimulation::impact_of_natural_gas_energy_increase_on_CR_energy_independence()
{
	// Announce which simulation is being run
	_OUTPUT_SIMULATION_HEADER();
	_OUTPUT_SIMULATION_PARAMS("Average yearly energy consumption");
	this->_output_delimit_result_group();

	// Run simulation
	this->compute_impact_of_natural_gas_energy_increase_on_CR_energy_independence();

	// Delimit simulations
	this->_output_delimit_simulations();
}

void GreenDealSimulation::compute_impact_of_natural_gas_energy_increase_on_CR_energy_independence() const
{
	// Output vars
	double years_energy_production[9];
	int independence_lost_year = -1;

	// Independence status
	bool isIndependent = true;

	// Energy production variable factors
	double photovoltaics_energy_production = this->simParams.energyYearlyPhotovoltaicsProduction;
	double wind_energy_production = this->simParams.energyYearlyWindEnergyProduction;
	double brown_coal_energy_production = this->simParams.energyYearlyBrownCoalEnergyProduction;
	double black_coal_energy_production = this->simParams.energyYearlyBlackCoalEnergyProduction;

	double natural_gas_production = this->simParams.energyYearlyNaturalGasProduction;
	double additional_emission = 0;


	// Yearly simulation
	for (int year = 0; year < 9; year++)
	{
		double annual_total_energy_production = 0;

		// Current year's energy increments
		photovoltaics_energy_production += this->simParams.energyYearlyPhotovoltaicsProductionIncrease;
		wind_energy_production += this->simParams.energyYearlyWindEnergyProductionIncrease;


		// Calculate emissions from the natural gas energy that is to be incremented in the current year
		double natural_gas_production_increase =
				natural_gas_production * this->simParams.energyYearlyNaturalGasProductionIncreasePercentage;

        // additional emission produced by increase of gas energy production (gas adds emission too)
        additional_emission = natural_gas_production_increase * this->simParams.naturalGasEnergyFootprintPerGwh;

		// Brown coal energy production after current year's reduction
		double brown_coal_energy_production_after_annual_reduction =
				brown_coal_energy_production
				- (
						this->simParams.energyYearlyBrownCoalEnergyProductionReduce +
                                (additional_emission/BROWN_COAL_ENERGY_FOOTPRINT_PER_GWH)
				);

		// Current year's energy decrements
		brown_coal_energy_production =
				std::max(0.0, brown_coal_energy_production_after_annual_reduction);

		black_coal_energy_production =
				std::max(0.0, black_coal_energy_production - this->simParams.energyYearlyBlackCoalEnergyProductionReduce);


		if (brown_coal_energy_production_after_annual_reduction >= 0.0)
		{
			natural_gas_production += natural_gas_production_increase;
		}
		else
		{
			// Calculate how much brown coal production was reduced this year
			double last_brown_coal_production_reduction =
					this->simParams.energyYearlyBrownCoalEnergyProductionReduce
                    +  (additional_emission/BROWN_COAL_ENERGY_FOOTPRINT_PER_GWH)
					- std::abs(brown_coal_energy_production_after_annual_reduction);

			// If there was any brown coal production left, calculate how much gas could be made in its stead
			if (last_brown_coal_production_reduction > 0)
			{
				// Calculate how much CO2 was reduced by that
				double available_CO2_threshold = last_brown_coal_production_reduction * BROWN_COAL_ENERGY_FOOTPRINT_PER_GWH;

				// Calculate how much natural gas energy can be made without exceeding that CO2 threshold
				double max_natural_gas_increase = available_CO2_threshold / NATURAL_GAS_ENERGY_FOOTPRINT_PER_GWH;

				natural_gas_production += max_natural_gas_increase;
			}
		}

		// Compute yearly total for current year
		annual_total_energy_production +=
				this->simParams.energyAverageYearlyProductionWithoutNaturalGas
				+ photovoltaics_energy_production
				+ wind_energy_production
				+ brown_coal_energy_production
				+ black_coal_energy_production
				+ natural_gas_production;

		// Record results
		years_energy_production[year] = annual_total_energy_production;

		std::cout << "Energy production for year 202" << year + 1 << " = " << \
            annual_total_energy_production << " GWh";
		std::cout << std::endl;

		std::cout << "Energy consumption = " << this->simParams.energyYearlyConsumption << " GWh";
		std::cout << std::endl;

		std::cout << "Resulting energy balance = " << \
            annual_total_energy_production - this->simParams.energyYearlyConsumption << " GWh";
		std::cout << std::endl;

		// Determine whether still independent
		double energyLost = this->simParams.energyYearlyConsumption - annual_total_energy_production;
		bool losingIndependence = energyLost > ENERGY_LOSS_THRESHOLD;

		if (losingIndependence && isIndependent) // check for isIndependent to only run statement body once
		{
			isIndependent = false;
			independence_lost_year = year + 1;
		}

		// Delimit respective years
		this->_output_delimit_result_group();
	}

	// Output
	this->_output_simulation_additional_results();
	if (independence_lost_year != -1)
	{
		std::cout << "\tEnergy independence has been lost in year 202" << independence_lost_year;
		std::cout << std::endl;
	}
	else
	{
		std::cout << "\tEnergy independence has persevered" << std::endl;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GreenDealSimulation::maximum_possible_decarbonization_to_preserve_CR_energy_independence()
{
	// Announce which simulation is being run
	_OUTPUT_SIMULATION_HEADER();
	_OUTPUT_SIMULATION_PARAMS("Average yearly energy consumption");
	this->_output_delimit_result_group();

	// Run simulation
	this->compute_maximum_possible_decarbonization_that_wont_affect_CR_energy_independence();

	this->_output_delimit_simulations();
}

void GreenDealSimulation::compute_maximum_possible_decarbonization_that_wont_affect_CR_energy_independence() const
{
	// Output vars
	double years_energy_production[9];
	double total_energy_production_loss = 0;

	bool isIndependent = true;

	// Energy production variable factors
	double photovoltaics_energy_production = this->simParams.energyYearlyPhotovoltaicsProduction;
	double wind_energy_production = this->simParams.energyYearlyWindEnergyProduction;
	double brown_coal_energy_production = this->simParams.energyYearlyBrownCoalEnergyProduction;
	double black_coal_energy_production = this->simParams.energyYearlyBlackCoalEnergyProduction;

	double brown_coal_truncated_reduction = 0;

	// Yearly simulation
	for (int year = 0; year < 9; year++)
	{
		double annual_total_energy_production = 0;

		// Energy increases
		photovoltaics_energy_production += this->simParams.energyYearlyPhotovoltaicsProductionIncrease;
		wind_energy_production += this->simParams.energyYearlyWindEnergyProductionIncrease;

		double brown_coal_production_after_annual_reduction =
				brown_coal_energy_production - this->simParams.energyYearlyBrownCoalEnergyProductionReduce;

		if (brown_coal_production_after_annual_reduction < 0)
		{
			brown_coal_truncated_reduction =
					this->simParams.energyYearlyBrownCoalEnergyProductionReduce
						- std::abs(brown_coal_production_after_annual_reduction);
		}

		// Energy decreases
		brown_coal_energy_production =
				std::max(0.0, brown_coal_production_after_annual_reduction);
		black_coal_energy_production =
				std::max(0.0,
						 black_coal_energy_production - this->simParams.energyYearlyBlackCoalEnergyProductionReduce);

		// Compute yearly total for current year
		annual_total_energy_production +=
				this->simParams.energyAverageYearlyProduction
				+ photovoltaics_energy_production
				+ wind_energy_production
				+ brown_coal_energy_production
				+ black_coal_energy_production;

		// Record results
		years_energy_production[year] = annual_total_energy_production;

		// Final year - compute how much the energy production is under consumption
		if (year == 8)
		{
			// Determine whether any energy was lost
			bool energyLost = annual_total_energy_production < this->simParams.energyYearlyConsumption;
			if (energyLost) // Account for some calculation deviation (1 serves as a minimum threshold)
			{
				// Record the amount of lost energy
				double energy_loss_amount =
						this->simParams.energyYearlyConsumption - annual_total_energy_production;

				if (energy_loss_amount > ENERGY_LOSS_THRESHOLD)
				{
					isIndependent = false;
					total_energy_production_loss = energy_loss_amount;
				}
			}
		}
	}

	// If the country is energy independent at this point, do not calculate max possible coal decarbonization rates
	if (isIndependent)
	{
		// Output
		std::cout << "Energy independence preserved - no need to reduce decarbonization rates!" << std::endl;
		return;
	}

	// If the full amount of brown coal reduction was not applied at the time of the last reduction
	// (reduction truncated), add the partial reduction (loss of energy) to brown coal here
	if (brown_coal_truncated_reduction > 0)
	{
		total_energy_production_loss += (BROWN_COAL_ENERGY_YEARLY_PRODUCTION_REDUCE - brown_coal_truncated_reduction);
	}

	// Compute the maximum possible coal decarbonization (reduction factors)
	double brown_coal_required_increase = CO2_YEARLY_FOOTPRINT_REDUCE_BROWN_COAL_RATIO * total_energy_production_loss;
	double brown_coal_required_increase_footprint = BROWN_COAL_ENERGY_FOOTPRINT_PER_GWH * brown_coal_required_increase;
	double brown_coal_required_increase_footprint_per_simulation_year = brown_coal_required_increase_footprint / 9;
	double brown_coal_max_possible_decarbonization =
			this->simParams.brownCoalYearlyFootprintReduce - brown_coal_required_increase_footprint_per_simulation_year;

	double black_coal_required_increase = CO2_YEARLY_FOOTPRINT_REDUCE_BLACK_COAL_RATIO * total_energy_production_loss;
	double black_coal_required_increase_footprint = BLACK_COAL_ENERGY_FOOTPRINT_PER_GWH * black_coal_required_increase;
	double black_coal_required_increase_footprint_per_simulation_year = black_coal_required_increase_footprint / 9;
	double black_coal_max_possible_decarbonization =
			this->simParams.blackCoalYearlyFootprintReduce - black_coal_required_increase_footprint_per_simulation_year;

	// Output
	std::cout << "Final amount of energy loss: " << total_energy_production_loss << " GWh" << std::endl;
	std::cout << "Maximum possible decarbonization per year to preserve energy independence:  " << std::endl;
	std::cout << "\t" << " - Brown Coal = " << brown_coal_max_possible_decarbonization << " Mt CO2" << std::endl;
	std::cout << "\t" << " - Black Coal = " << black_coal_max_possible_decarbonization << " Mt CO2" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GreenDealSimulation::required_renewable_energy_increase_to_save_CR_energy_independence()
{
    // Announce which simulation is being run
    _OUTPUT_SIMULATION_HEADER();
	_OUTPUT_SIMULATION_PARAMS("Average yearly energy consumption");
    this->_output_delimit_result_group();

    // Run simulation
    this->compute_required_renewable_energy_increase_to_save_CR_energy_independence(
			this->simParams.energyYearlyConsumption);

    // Delimit simulations
	this->_output_delimit_simulations();
}

void GreenDealSimulation::compute_required_renewable_energy_increase_to_save_CR_energy_independence(double yearly_energy_consumption) const
{
    // Output vars
    double years_energy_production[9];
    double years_required_renewable_increase[9];

    double total_production_lost = 0;
    double total_year_renewable = 0;
    double required_renewable_increase = 0;

    // Energy production variable factors
    double photovoltaics_energy_production = this->simParams.energyYearlyPhotovoltaicsProduction;
    double wind_energy_production = this->simParams.energyYearlyWindEnergyProduction;
    double brown_coal_energy_production = this->simParams.energyYearlyBrownCoalEnergyProduction;
    double black_coal_energy_production = this->simParams.energyYearlyBlackCoalEnergyProduction;

    // Yearly simulation
    for (int year = 0; year < 9; year++)
    {
        double annual_total_energy_production = 0;
        // Energy increases
        photovoltaics_energy_production += this->simParams.energyYearlyPhotovoltaicsProductionIncrease;
        wind_energy_production += this->simParams.energyYearlyWindEnergyProductionIncrease;

        // Energy decreases
        brown_coal_energy_production =
                std::max(0.0, brown_coal_energy_production - this->simParams.energyYearlyBrownCoalEnergyProductionReduce);
        black_coal_energy_production =
                std::max(0.0, black_coal_energy_production - this->simParams.energyYearlyBlackCoalEnergyProductionReduce);

        // Compute yearly total for current year
        annual_total_energy_production +=
                this->simParams.energyAverageYearlyProduction
                + photovoltaics_energy_production
                + wind_energy_production
                + brown_coal_energy_production
                + black_coal_energy_production;

        // Record results
        years_energy_production[year] = annual_total_energy_production;

        std::cout << "Energy production for year 202" << year + 1 << " = " << \
			annual_total_energy_production << " GWh";
        std::cout << std::endl;

        std::cout << "Energy consumption = " << yearly_energy_consumption << " GWh";
        std::cout << std::endl;

        std::cout << "Resulting energy balance = " << \
			annual_total_energy_production - yearly_energy_consumption << " GWh";
        std::cout << std::endl;

        // Determine whether still independent
        bool losingIndependence = annual_total_energy_production < yearly_energy_consumption;
        if (losingIndependence)
        {
            // independence is lost...needs to increase renewable energy production in current year
            total_production_lost = yearly_energy_consumption - annual_total_energy_production;

            total_year_renewable = this->simParams.renewableEnergyAverageYearlyProduction \
            + photovoltaics_energy_production + wind_energy_production;

            required_renewable_increase = total_production_lost/total_year_renewable;
            years_required_renewable_increase[year] = required_renewable_increase;

            // print result to stdout
            std::cout << "Required renewable energy increase in this year: " << required_renewable_increase * 100 << " %";
            std::cout << std::endl;
        }
        else
        {
            // independence not lost...do not need to increase renewable energy production in current year
            years_required_renewable_increase[year] = 0.0;

            std::cout << "Production is high enough, do not need increase renewable energy production";
            std::cout << std::endl;
        }

        // Delimit respective years
        this->_output_delimit_result_group();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GreenDealSimulation::_output_simulation_params(const std::string& paramInfo, ...) const
{
	if (this->simParams.graphicalOutput) return;

	std::cout << "USED PARAMETERS: " << std::endl;
	std::cout << "\t - " << paramInfo << std::endl;


	va_list params;
	va_start(params, paramInfo);

	auto param = va_arg(params, char*);
	while (param != "")
	{
		std::cout << "\t - " << std::string(param) << std::endl;
		param = va_arg(params, char*);
	}

	va_end(params);
}

void GreenDealSimulation::_output_delimit_result_group() const
{
	if (this->simParams.graphicalOutput) return;

	std::cout << "------------------------------------------------------" << std::endl;
}

void GreenDealSimulation::_output_simulation_additional_results() const
{
	if (this->simParams.graphicalOutput) return;

	std::cout << std::endl;
	std::cout << "Additional Results: " << std::endl;
}

void GreenDealSimulation::_output_delimit_simulations() const
{
	if (this->simParams.graphicalOutput) return;

	std::cout << "#########################################################################" << std::endl;
	std::cout << std::endl;
}
