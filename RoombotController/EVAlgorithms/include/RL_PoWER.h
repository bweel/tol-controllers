#ifndef RL_POWER_H
#define	RL_POWER_H

#include "EVAlgorithm.h"

#include "Globals.h"
#include "Trial.h"
#include "Spline.h"
#include "Values.h"

#include <string>

class RL_PoWER
:
virtual public EVAlgorithm
{
  public:

    RL_PoWER(unsigned int seed, const std::string & p_path, const std::string & d_path, double time_step, double angular_velocity, unsigned int evaluations, std::size_t s_size);
	RL_PoWER(const std::string &, double, double, std::size_t);

	virtual ~RL_PoWER();

	void setParameter(std::size_t, double);
	double getParameter(std::size_t);

	void reset();
	void update();

	bool nextEvaluation();
	inline void setEvaluationFitness(double);
	inline void setEvaluationFitnessAlt(const std::string &);

	void save();
    
    unsigned int getGeneration();
    unsigned int getEvaluation();
    
    virtual void setInitialMinds(boost::ptr_vector<MindGenome> genomes,std::size_t numMotors,std::size_t organismSize);
    virtual boost::ptr_vector<MindGenome> getRandomInitialMinds();
    
    
  private:

	bool _flag;

	std::string _directory;

	std::size_t _ev_step;
	std::size_t _ev_steps;

    std::size_t numSplines;
    unsigned int evaluations;
    
    std::vector<std::vector<double> > initialParameters;
    
    utils::Random *_random;
	POWER::Globals * _globals;
	POWER::Trial * _trial;
	POWER::Values _evaluation;
};

#endif	/* RL_POWER_H */