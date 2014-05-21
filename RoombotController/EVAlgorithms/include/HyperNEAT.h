#ifndef HYPERNEAT_H
#define	HYPERNEAT_H

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

/* STL Includes */
#include <climits>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

class HyperNEAT
:
virtual public EVAlgorithm
{
  public:
	HyperNEAT(unsigned int, const std::string &, const std::string &, const transforms::Vector_3 &, const std::vector<transforms::Vector_3> &, const int numMotors, std::size_t extraParameters = 0);
	virtual ~HyperNEAT();

	void setParameter(std::size_t, double);
	double getParameter(std::size_t);

	void reset();
	void update();

	bool nextEvaluation();
	void setEvaluationFitness(double);
	void setEvaluationFitnessAlt(const std::string &);

	void save();
    
    unsigned int getGeneration();
    unsigned int getEvaluation();

  private:

	typedef NEAT::GeneticNodeGene NodeGene;
	typedef std::vector<NodeGene> Genome;
	typedef NEAT::LayeredSubstrate<double> Substrate;
	typedef NEAT::LayeredSubstrateInfo SubstrateInfo;
	typedef NEAT::GeneticIndividual Individual;
	typedef NEAT::GeneticPopulation Population;

	static const std::string LAYER_INPUT_NAME;
	static const std::string LAYER_HIDDEN_NAME;
	static const std::string LAYER_OUTPUT_NAME;

	NEAT::Globals * _globals;
	boost::filesystem::path _directory;

	std::ofstream _log_i;
	std::ofstream _log_c;
	std::ofstream _log_s;

	std::vector< NEAT::Node > _mappings_in;
	std::vector< NEAT::Node > _mappings_out;

	int _i_index_last;
	int _g_index_prev;
	int _g_index_last;

	Population _population;
	Substrate _substrate;

	void set_state();
//	TiXmlElement * get_state();

	/**
	 * Calculates the size of the substrate layer needed to address the robot servos
	 * For the moment we handle only robots that can be represented two dimensionally
	 * @param Robot size in the three dimensions
	 * @return Sizes of the robot substrate layer
	 */
	JGTL::Vector2<int> initMotorLayerDimensions(const transforms::Vector_3 & organismSize, const int numMotors);

	/**
	 * Calculates the size of the substrate layer needed to address the robot
	 * parameters taking into account the number of parameters and the size
	 * of the substrate used to address the robot servos
	 * @param Parameters size
	 * @param Robot substrate sizes
	 * @return Sizes of the parameters substrate layer
	 */
	JGTL::Vector2<int> initParameterLayerDimensions(std::size_t parameterSize, const JGTL::Vector2<int> & organismLayerSize);

	/**
	 * Sets up information about each substrate layer, starting from the input
	 * layer, then the hidden layers and at last the output layer
	 * We use the same size for each layer, the names for each layer are defined
	 * in the static variables of this class
	 * @param Hidden layers
	 * @param Layer size
	 * @return Substrate information
	 */
	SubstrateInfo initialiseSubstrateInfo(std::size_t numHiddenLayers, const JGTL::Vector2<int> & layerDimensions);

	/**
	 * Creates a model genome made only of node genes
	 * @param
	 * @return
	 */
	Genome _init_genome(std::size_t);

	/**
	 * Creates a genetic population of size n using a model genome made only of
	 * node genes and populating it with link genes with random weights
	 * Each individual in the population is a CPPN which will be used to instantiate
	 * a phenotype for evaluation
	 * @param Population size
	 * @param Model genome
	 * @return Genetic population
	 */
	Population _init_population(std::size_t, const Genome &);

	std::vector< JGTL::Vector3<int> > _init_mappings(const std::vector<transforms::Vector_3> & indexes, const JGTL::Vector2<int> & motorLayerDimensions, const int numMotors,
                                                     std::size_t parameterSize, const JGTL::Vector2<int> & parameterLayerDimensions);

	void save_individual(const int &, const boost::shared_ptr<NEAT::GeneticIndividual> &, const std::string &);
	void save_generation(const int &, const boost::shared_ptr<NEAT::GeneticGeneration> &, const std::string &);
};

#endif	/* HYPERNEAT_H */