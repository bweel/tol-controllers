//
//  SplineNeat.cpp
//  ToLController
//
//  Created by Berend Weel on 10/02/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "SplineNeat.h"
#include "MyMath.h"
#include "Trial.h"

#define SPLINE_MINIMUM_SIZE 2
#define SPLINE_MAXIMUM_SIZE 100

SplineNeat::SplineNeat(unsigned int seed,
                       const std::string & parametersPath,                // Path to the parameter file for HyperNEAT
                       const std::string & logDirectory,                  // Logging Directory
                       const std::vector<transforms::Vector_3> & indexes, // The indices of the modules int he organism
                       const int numMotors,                               // The number of motors per module
                       double timeStep,                                   // The time step of the simulator
                       double angularVelocity,                            // The speed of the motor
                       unsigned int numModules)                            // The number of splines in a policy
: EVAlgorithm::EVAlgorithm("SplineNEAT")
                        , globals(parametersPath.empty() ? NEAT::Globals::init() : NEAT::Globals::init(parametersPath))
                        , directory(logDirectory)
                        , evaluationCurrentStep(0)
                        , evaluationTotalSteps(0)
                        , currentIndividual(0)
                        , currentGeneration(0)
                        , numSplines(numModules * numMotors)
                        , intervalMinimumLength(SPLINE_MINIMUM_SIZE)
                        , intervalMaximumLength(SPLINE_MAXIMUM_SIZE)
                        , modulePositions(indexes)
                        , interval(POWER::Trial::INTERVAL_START, POWER::Trial::INTERVAL_END, SPLINE_MINIMUM_SIZE)

{
    globals->seedRandom(seed);

    // Check and initialize some spline parameters
    if (0.0 >= timeStep) {
		throw std::domain_error("Time Step Cannot Be <= 0.0");
	}
    
	if (0.0 >= angularVelocity) {
		throw std::domain_error("Angular Velocity Cannot Be <= 0.0");
	}
    
	double time = (utils::MyMath::PI_DOUBLE / angularVelocity);
	if (time < (timeStep * 2.0)) {
		throw std::domain_error("Time Must Be At Least == 2 * Time Steps");
	}
    
    // Determine the number of steps an evaluation has depending on the speed of the motor
	evaluationTotalSteps = static_cast<unsigned int> (round(time / timeStep));
    
    // Determine how often to update the length of the interval
    double maxGeneration = globals->getParameterValue("MaxGenerations");
    intervalLengthUpdateFrequency = (intervalMaximumLength - intervalMinimumLength) ? round(maxGeneration / (double)(intervalMaximumLength - intervalMinimumLength)) : 1;
    std::cout << "Updating the spline length every " << intervalLengthUpdateFrequency << " generations" << std::endl;
    
    // Initialize the population of ccpn's
	std::size_t populationSize(static_cast<std::size_t> (globals->getParameterValue("PopulationSize")));
    
    Genome genome = initializeGenome(numMotors);
    population = initializePopulation(populationSize, genome);
    
    boost::shared_ptr<Individual> individual = population.getIndividual(currentIndividual, currentGeneration);
    cppn = individual->spawnFastPhenotypeStack<double>();

    // use the cppn to initialize the starting evaluation
    initializePolicy();
    evaluation = (currentPolicy->parameters(interval, evaluationTotalSteps) -0.5) * 2;
    
	if (!boost::filesystem::exists(directory) || !boost::filesystem::is_directory(directory)) {
		boost::filesystem::create_directories(directory);
	}
}

SplineNeat::~SplineNeat() {
}

void SplineNeat::setParameter(std::size_t index, double value) {
    // The policies do not use any parameters
}

double SplineNeat::getParameter(std::size_t index) {
    return evaluation(index, evaluationCurrentStep);
}

void SplineNeat::setInitialMinds(boost::ptr_vector<MindGenome> genomes) {
    
}

boost::ptr_vector<MindGenome> SplineNeat::getRandomInitialMinds() {
    boost::ptr_vector<MindGenome> minds;
    
    return minds;
}

void SplineNeat::reset() {
}

void SplineNeat::update() {
    evaluationCurrentStep += 1;
    
	if (evaluationCurrentStep >= evaluationTotalSteps) {
		evaluationCurrentStep = 0;
	}
}

bool SplineNeat::nextEvaluation() {
    evaluationCurrentStep = 0;
    currentIndividual += 1;
    
    if (population.getIndividualCount(currentGeneration) == currentIndividual) {
        int nextGeneration = currentGeneration + 1;

        if (nextGeneration == static_cast<int> (globals->getParameterValue("MaxGenerations"))) {
            return false;
        }
        
        population.adjustFitness();
		population.produceNextGeneration();
        
        std::size_t intervalLength = interval.size();
        
		if ((intervalLength < intervalMaximumLength) && (!(currentGeneration % intervalLengthUpdateFrequency))) {
			interval.resize(intervalLength + 1);
		}
        
        population.cleanupOld(nextGeneration);
        
        currentIndividual = 0;
        currentGeneration = nextGeneration;
    }
    
    
    boost::shared_ptr<Individual> individual = population.getIndividual(currentIndividual, currentGeneration);
    cppn = individual->spawnFastPhenotypeStack<double>();

    initializePolicy();
    evaluation = (currentPolicy->parameters(interval, evaluationTotalSteps) -0.5) * 2;
    
    return true;
}

void SplineNeat::initializePolicy(){
    std::size_t splineLength = interval.size();
    
    // Create a valarray of numSplines row of size values
    std::valarray<double> splines(numSplines * splineLength);
    
    // For each module
    for ( std::size_t i=0; i < numSplines; i++ ) {
        // set the X and Y positions on the cppn
        int x = modulePositions[i].x();
        int y = modulePositions[i].y();
        cppn.setValue("Bias", 0.3);
        cppn.setValue("X", x);
        cppn.setValue("Y", y);
        for ( unsigned int j = 0; j < splineLength; j++ ) {
            // set the 'time' value on the cppn
            cppn.setValue("T", interval[j]);
            
            // calculate the next output
            cppn.update();
            
            splines[ i*splineLength + j ] = 0.5 * (cppn.getValue("Output_Y") + 1);
        }
        cppn.reinitialize();
    }
    
    // Convert the valarray to a Values class that Policy uses
    POWER::Values parameters(numSplines, splineLength, splines);
    
    // Initialize the policy. Variance is set to 0 as it is not used in this context, neither is Index (currentIndividual)
    currentPolicy = new POWER::Policy(currentIndividual, 0, interval, parameters);
}

/**
 * Creates a model genome made only of node genes
 * @param hiddenLayers - the number of hidden layers
 * @return a model genome
 */
SplineNeat::Genome SplineNeat::initializeGenome(unsigned int numMotors) {
    Genome result;
    
	/*
	 * These are the input node genes used to query the CPPN about
     * Spline parameters
     *
     * Inputs:
     * Bias node
     * X position of the module
     * Y position of the module
     * T time position for the spline
	 */
	result.push_back(NodeGene("Bias", "NetworkSensor", 0, false));
	result.push_back(NodeGene("X", "NetworkSensor", 0, false));
	result.push_back(NodeGene("Y", "NetworkSensor", 0, false));
	result.push_back(NodeGene("T", "NetworkSensor", 0, false));
    
#if DEBUG_USE_DELTAS
	result.push_back(NodeGene("DeltaX", "NetworkSensor", 0, false));
	result.push_back(NodeGene("DeltaY", "NetworkSensor", 0, false));
#endif
    
    /*
     * Output of the cppn
     * This is the y position of the spline at time t
     */
	result.push_back(NodeGene("Output_Y", "NetworkOutputNode", 1, false, ACTIVATION_FUNCTION_SIGMOID));
    
	return result;
}

/**
 * Creates a genetic population of size n using a model genome made only of
 * node genes and populating it with link genes with random weights
 * Each individual in the population is a CPPN which will be used to instantiate
 * a phenotype for evaluation
 * @param Population size
 * @param Model genome
 * @return Genetic population
 */
SplineNeat::Population SplineNeat::initializePopulation(std::size_t populationSize, const Genome & genome) {
    Population result;
    
	for (std::size_t index = 0; index < populationSize; index++) {
		boost::shared_ptr<Individual> individual(new Individual(genome, true, 1.0));
        
		result.addIndividual(individual);
	}
    
	return result;
}

void SplineNeat::setEvaluationFitness(double value) {
	boost::shared_ptr<Individual> individual = population.getIndividual(currentIndividual, currentGeneration);
    
	individual->setFitness(value);
}

void SplineNeat::setEvaluationFitnessAlt(const std::string & string) {
	boost::shared_ptr<Individual> individual = population.getIndividual(currentIndividual, currentGeneration);
    
	individual->setUserData(string);
}

void SplineNeat::save() {
    int d_index, dumpModulo, d_remainder;
    
	dumpModulo = globals->getParameterValue("GenerationDumpModulo");
	d_remainder = currentGeneration % dumpModulo;
    
	if (d_remainder) {
		for (d_index = (currentGeneration - d_remainder); d_index < currentGeneration; d_index++) {
			saveIndividual(d_index, population.getBestIndividualOfGeneration(d_index), directory.string());
			saveGeneration(d_index, population.getGeneration(d_index), directory.string());
		}
	}
    
	population.cleanupOld(currentGeneration - 1);
}

void SplineNeat::saveIndividual(const int & index, const boost::shared_ptr<NEAT::GeneticIndividual> & individual, const std::string & path)
{
	boost::filesystem::path logDirectory(path);
    
	logDirectory /= ("Generation_" + boost::lexical_cast<std::string>(index) + "_Best_Individual.xml");
    
	TiXmlDocument document(logDirectory.string());
	TiXmlElement * element = new TiXmlElement("Individual");
    
	individual->dump(element, true);
	element->SetAttribute("GenerationID", index);
	document.LinkEndChild(element);
    
	document.SaveFile();
#ifdef DEBUG_ALGORITHM
	std::cout << "Saved Generation " << index << " Best Individual" << std::endl;
#endif
}

void SplineNeat::saveGeneration(const int & index, const boost::shared_ptr<NEAT::GeneticGeneration> & generation, const std::string & path)
{
	boost::filesystem::path logDirectory(path);
    
	logDirectory /= ("Generation_" + boost::lexical_cast<std::string>(index) + ".xml");
    
	TiXmlDocument document(logDirectory.string());
	TiXmlElement * element = new TiXmlElement("Generation");
    
	generation->dump(element, true);
	document.LinkEndChild(element);
    
	document.SaveFile();
#ifdef DEBUG_ALGORITHM
	std::cout << "Saved Generation: " << index << std::endl;
#endif
}

unsigned int SplineNeat::getGeneration() {
    return currentGeneration;
}

unsigned int SplineNeat::getEvaluation() {
    return currentIndividual;
}