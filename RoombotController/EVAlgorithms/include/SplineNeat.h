//
//  SplineNeat.h
//  ToLController
//
//  Created by Berend Weel on 10/02/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef ToLController_SplineNeat_h
#define ToLController_SplineNeat_h

#include "EVAlgorithm.h"

#include "Vector_3.h"

/* Libraries Includes*/
#include "NEAT.h"

#include "JGTL_Vector3.h"
#include "JGTL_Vector2.h"

/* BOOST Includes */
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/shared_ptr.hpp>

#include "Globals.h"
#include "Values.h"
#include "Policy.h"
#include "Interval.h"

#include <string>

class SplineNeat
:
virtual public EVAlgorithm
{
public:
    
    SplineNeat(unsigned int seed,
               const std::string & parametersPath,                // Path to the parameter file for HyperNEAT
               const std::string & logDirectory,                  // Logging Directory
               const std::vector<transforms::Vector_3> & indexes, // The indices of the modules in the organism
               const int numMotors,                               // The number of motors per module
               double timeStep,                                   // The time step of the simulator
               double angularVelocity,                            // The speed of the motor
               unsigned int numModules);                          // The number of modules in the organism
    
	virtual ~SplineNeat();
    
	void setParameter(std::size_t, double);
	double getParameter(std::size_t);
    
	void reset();
	void update();
    
	bool nextEvaluation();
	inline void setEvaluationFitness(double fitness);
	inline void setEvaluationFitnessAlt(const std::string &string);
    
	void save();
    
    unsigned int getGeneration();
    unsigned int getEvaluation();
private:
    void saveIndividual(const int & index, const boost::shared_ptr<NEAT::GeneticIndividual> & individual, const std::string & path);
    void saveGeneration(const int & index, const boost::shared_ptr<NEAT::GeneticGeneration> & generation, const std::string & path);
    
    typedef NEAT::GeneticNodeGene NodeGene;
	typedef std::vector<NodeGene> Genome;
	typedef NEAT::GeneticIndividual Individual;
	typedef NEAT::GeneticPopulation Population;
    typedef NEAT::FastNetwork<double> CPPN;

    
    /**
     * Initializes the next policy based on the current individual
     */
    void initializePolicy();
    
    /**
	 * Creates a model genome made only of node genes
     *
	 * @return a model genome
	 */
    Genome initializeGenome(unsigned int numMotors);
    
    /**
	 * Creates a genetic population of size n using a model genome made only of
	 * node genes and populating it with link genes with random weights
	 * Each individual in the population is a CPPN which will be used to instantiate
	 * a phenotype for evaluation
	 * @param Population size
	 * @param Model genome
	 * @return Genetic population
	 */
	Population initializePopulation(std::size_t populationSize, const Genome & genome);
   
	boost::filesystem::path directory;
    
	unsigned int evaluationCurrentStep;
	unsigned int evaluationTotalSteps;
    unsigned int currentIndividual;
    unsigned int currentGeneration;
    unsigned int numSplines;
    unsigned int intervalMinimumLength;
    unsigned int intervalMaximumLength;
    unsigned int intervalLengthUpdateFrequency;
    std::vector<transforms::Vector_3> modulePositions;
    
//    std::vector< std::string > mappingInput;
//	std::vector< std::string > mappingOutput;
    
	NEAT::Globals * globals;
    POWER::Interval interval;
    POWER::Policy * currentPolicy;
	POWER::Values evaluation;
    
    Population population;
    CPPN cppn;
};

#endif
