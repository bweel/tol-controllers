#ifndef EVALGORITHM_H
#define	EVALGORITHM_H

#include <cstddef>
#include <string>
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>

#include "MindGenome.h"

class LearningAlgorithm
{
  public:
	LearningAlgorithm(const std::string &);
	virtual ~LearningAlgorithm();

	/**
	 * Returns the algorithm name
	 * @return Name of the algorithm
	 */
	std::string getName() const;

	/**
	 * Set the n-th parameter to the desired value
	 * @param Index of parameter
	 * @param Value of parameter
	 */
	virtual void setParameter(std::size_t, double) = 0;

	/**
	 * Returns the value of the n-th parameter after being evaluated by the algorithm
	 * @param Index of parameter
	 * @return Computed value of parameter
	 */
	virtual double getParameter(std::size_t) = 0;

	virtual void reset() = 0;
	virtual void update() = 0;

	virtual bool nextEvaluation() = 0;
	virtual void setEvaluationFitness(double) = 0;
	virtual void setEvaluationFitnessAlt(const std::string &) = 0;
    
    virtual unsigned int getGeneration() = 0;
    virtual unsigned int getEvaluation() = 0;
    
    virtual void setInitialMinds(boost::ptr_vector<MindGenome> genomes,std::size_t numMotors,std::size_t organismSize) = 0;
    virtual boost::ptr_vector<MindGenome> getRandomInitialMinds() = 0;

	virtual void save() = 0;

  protected:
	std::string _name;
};

#endif	/* EVALGORITHM_H */