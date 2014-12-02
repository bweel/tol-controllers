//
//  PoWER.h
//  RoombotController
//
//  Created by Berend Weel on 02/07/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef __RoombotController__PoWER__
#define __RoombotController__PoWER__

#include <iostream>
#include "LearningAlgorithm.h"
#include "ParametersReader.h"
#include "Random.h"
#include "Logger.h"

class PoWER : public LearningAlgorithm {
public:
    
    const double VARIANCE = ParametersReader::get<int>("PoWER.Variance",0.008);
    const double VARIANCE_DECAY = ParametersReader::get<double>("PoWER.VarianceDecay", 0.98);
    
    const std::size_t SPLINE_SIZE_MIN = ParametersReader::get<int>("PoWER.SplineSizeMin", 2);
    const std::size_t SPLINE_SIZE_MAX = ParametersReader::get<int>("PoWER.SplineSizeMax", 20);
    const std::size_t RANKING_SIZE = ParametersReader::get<int>("PoWER.RankingSize", 10);
    
    const double INTERVAL_START = ParametersReader::get<double>("PoWER.IntervalStart",0.0);
    const double INTERVAL_END = ParametersReader::get<double>("PoWER.IntervalEnd",1.0);
    const double FITNESS_EXP = ParametersReader::get<double>("PoWER.FitnessExponent",6.0);
    const double EPSILON = ParametersReader::get<double>("PoWER.Epsilon",1e-10);
    
    PoWER(unsigned int seed, const std::string & loggingPath, double timeStep, double angularVelocity, unsigned int evaluations, std::size_t numSplines);
    virtual ~PoWER();
    
	/**
	 * Set the n-th parameter to the desired value
	 * @param Index of parameter
	 * @param Value of parameter
	 */
	virtual void setParameter(std::size_t, double);
    
	/**
	 * Returns the value of the n-th parameter after being evaluated by the algorithm
	 * @param Index of parameter
	 * @return Computed value of parameter
	 */
	virtual double getParameter(std::size_t);
    
	virtual void reset();
	virtual void update();
    
	virtual bool nextEvaluation();
	virtual void setEvaluationFitness(double);
	virtual void setEvaluationFitnessAlt(const std::string &);
    
    virtual unsigned int getGeneration();
    virtual unsigned int getEvaluation();
    
    virtual void setInitialMinds(boost::ptr_vector<MindGenome> genomes,std::size_t numMotors,std::size_t organismSize);
    virtual boost::ptr_vector<MindGenome> getRandomInitialMinds();
    
	virtual void save();
    
private:
    unsigned int evaluation;
    unsigned int numSplines;
    
    std::vector<std::vector<double> > initialParameters;
    
    Utils::Random *random;
    log4cpp::Category& logger;
};

#endif /* defined(__RoombotController__PoWER__) */
