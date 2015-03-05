//
//  PoWER.cpp
//  RoombotController
//
//  Created by Berend Weel on 02/07/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "PoWER.h"

#include "Random.h"
#include "MatrixGenome.h"
#include "Logger.h"

PoWER::PoWER(unsigned int seed,
             const std::string & loggingPath,
             double timeStep,
             double angularVelocity,
             unsigned int evaluations,
             std::size_t numSplines)
: LearningAlgorithm("PoWER"),
logger(Logger::getInstance("RLPower")) {
    random = Utils::Random::getInstance();
}

PoWER::~PoWER() {
    
}

/**
 * Set the n-th parameter to the desired value
 * @param Index of parameter
 * @param Value of parameter
 */
void PoWER::setParameter(std::size_t, double) {
    
}

/**
 * Returns the value of the n-th parameter after being evaluated by the algorithm
 * @param Index of parameter
 * @return Computed value of parameter
 */
double PoWER::getParameter(std::size_t) {
    return 0;
}

void PoWER::reset() {
    
}

void PoWER::update() {
    
}

bool PoWER::nextEvaluation() {
    return true;
}

void PoWER::setEvaluationFitness(double) {
    
}

void PoWER::setEvaluationFitnessAlt(const std::string &) {
    
}

unsigned int PoWER::getGeneration() {
    return evaluation;
}

unsigned int PoWER::getEvaluation() {
    return evaluation;
}

void PoWER::setInitialMinds(boost::ptr_vector<MindGenome> genomes,std::size_t numMotors,std::size_t organismSize) {
    double sqrtvariance = std::sqrt(VARIANCE);
	std::size_t min_size = SPLINE_SIZE_MIN;
    
    MatrixGenome *genes = static_cast<MatrixGenome*>(&genomes[0]);
    initialParameters = genes->getMatrix();
    
    std::size_t totalParameters = organismSize * numMotors;
    logger.infoStream() << "Setting initial mind of size: " << initialParameters.size() << " actual number size needed: " << totalParameters;
    if(initialParameters.size() < totalParameters) {
        // If the size of the parent's mind is too low
        // Add extra splines here
        std::size_t extraParameters = totalParameters - initialParameters.size();
        
        logger.infoStream() << "Parent mind did not have enough parameters, adding " << extraParameters << " parameter vectors";
        
        for(std::size_t i=0;i<extraParameters;i++) {
            std::vector<double> parameters(min_size, 0.5);
            
            for(std::size_t param = 0; param < min_size; param++) {
                parameters[param] += sqrtvariance * random->normal_real();
            }
            initialParameters.push_back(parameters);
        }
    }
    
//  _trial = new POWER::Trial(_random, numSplines, min_size, max_size, rank_size, evaluations, variance, variance_decay, initialParameters);
//	_evaluation = (_trial->policy(_ev_steps) - 0.5) * 2.0;
}

boost::ptr_vector<MindGenome> PoWER::getRandomInitialMinds() {
    double sqrtvariance = std::sqrt(VARIANCE);
    std::size_t min_size = static_cast<std::size_t> (SPLINE_SIZE_MIN);
    
    std::vector<std::vector<double> > parameters(numSplines, std::vector<double>(min_size, 0.5));
    
    for (std::size_t spline = 0; spline < numSplines; spline++) {
        for(std::size_t param = 0; param < min_size; param++) {
            parameters[spline][param] += sqrtvariance * random->normal_real();
        }
    }
    
    MatrixGenome *genome = new MatrixGenome();
    genome->setValues(parameters);
    
    boost::ptr_vector<MindGenome> minds;
    minds.push_back(genome);
    return minds;
}

void PoWER::save() {
    
}