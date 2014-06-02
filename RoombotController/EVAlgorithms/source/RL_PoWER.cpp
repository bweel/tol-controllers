#include "RL_PoWER.h"

#include "MyMath.h"
#include "MatrixGenome.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>
#include <stdexcept>

RL_PoWER::RL_PoWER(unsigned int seed,
				   const std::string & p_path,
				   const std::string & d_path,
				   double time_step,
				   double angular_velocity,
                   unsigned int evaluations,
				   std::size_t numSplines)
:
EVAlgorithm::EVAlgorithm("RL_PoWER"),
_flag(false),
_directory(d_path),
_ev_step(0),
_ev_steps(0),
_globals((p_path.empty()) ? POWER::Globals::init() : POWER::Globals::load(p_path)),
numSplines(numSplines),
evaluations(evaluations)
{
	if (0.0 >= time_step) {
		throw std::domain_error("Time Step Cannot Be <= 0.0");
	}

	if (0.0 >= angular_velocity) {
		throw std::domain_error("Angular Velocity Cannot Be <= 0.0");
	}

	double time = (utils::MyMath::PI_DOUBLE / angular_velocity);

	if (time < (time_step * 2.0)) {
		throw std::domain_error("Time Must Be At Least == 2 Time Steps");
	}

	_ev_steps = static_cast<std::size_t> (round(time / time_step));
   
    _random = new utils::Random(seed);
   
}

RL_PoWER::RL_PoWER(const std::string & path, double time_step, double angular_velocity, std::size_t s_size)
:
EVAlgorithm::EVAlgorithm("RL_PoWER"),
_flag(false),
_ev_step(0)
{
	// Recalculating these values because we may be using a different time step

	if (0.0 >= time_step) {
		throw std::domain_error("Time Step Cannot Be <= 0.0");
	}

	if (0.0 >= angular_velocity) {
		throw std::domain_error("Angular Velocity Cannot Be <= 0.0");
	}

	double time = (utils::MyMath::PI_DOUBLE / angular_velocity);

	if (time < (time_step * 2.0)) {
		throw std::domain_error("Time Must Be At Least == 2 Time Steps");
	}

	_ev_steps = static_cast<std::size_t> (round(time / time_step));

	TiXmlDocument doc(path);

	if (!doc.LoadFile()) {
		throw std::runtime_error("XML Error: Loading File " + path);
	}

	const TiXmlElement * rl = doc.FirstChildElement(_name);

	if (!rl) {
		throw std::runtime_error("XML Error: Element Parsing");
	}

	std::size_t steps = 0;

	if (rl->QueryValueAttribute<std::size_t>("Evaluation_Steps", &steps)) {
		throw std::runtime_error("XML Error: Attribute Parsing");
	}

	if (steps != _ev_steps) {
		std::cout << "Experiment Carried Out With Different Robot" << std::endl;
	}

	// Checking if current servo size is equal to the last used

	std::size_t _s_size = 0;

	if (rl->QueryValueAttribute<std::size_t>("Servo_Size", &_s_size)) {
		throw std::runtime_error("XML Error: Attribute Parsing");
	}

	if (_s_size != s_size) {
		throw std::runtime_error("");
	}

	const TiXmlElement * trial = rl->FirstChildElement("Trial");

	if (!trial) {
		throw std::runtime_error("XML Error: Element Parsing");
	}

	_globals = POWER::Globals::init();
	_trial = new POWER::Trial(* trial);
	_evaluation = (_trial->policy(_ev_steps) - 0.5) * 2.0;
}

RL_PoWER::~RL_PoWER()
{
	if (_trial) {
		delete (_trial);
	}

	if (_globals) {
		_globals->deinit();
	}
}

void RL_PoWER::setParameter(std::size_t index, double value)
{
#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Set Parameter" << " "
			<< index << " "
			<< value
			<< std::endl;
#endif
}

double RL_PoWER::getParameter(std::size_t index)
{
#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Get Parameter" << " "
			<< index << " "
			<< _evaluation(index, column)
			<< std::endl;
#endif

	return _evaluation(index, _ev_step);
}

void RL_PoWER::setInitialMinds(boost::ptr_vector<MindGenome> genomes,std::size_t numMotors,std::size_t organismSize) {
    double variance = _globals->parameter(POWER::Trial::VARIANCE_NAME);
	double variance_decay = _globals->parameter(POWER::Trial::VARIANCE_DECAY_NAME);
    double sqrtvariance = std::sqrt(_globals->parameter(POWER::Trial::VARIANCE_NAME));
	std::size_t min_size = static_cast<std::size_t> (_globals->parameter(POWER::Trial::V_SIZE_MIN_NAME));
	std::size_t max_size = static_cast<std::size_t> (_globals->parameter(POWER::Trial::V_SIZE_MAX_NAME));
	std::size_t rank_size = static_cast<std::size_t> (_globals->parameter(POWER::Trial::RANKING_SIZE_NAME));
    //	std::size_t ev_size = static_cast<std::size_t> (_globals->parameter(POWER::Trial::EVALUATION_SIZE_NAME));
   
    MatrixGenome *genes = static_cast<MatrixGenome*>(&genomes[0]);
    initialParameters = genes->getMatrix();
    
    std::size_t totalParameters = organismSize * numMotors;
    std::cout << "Setting initial mind of size: " << initialParameters.size() << " actual number size needed: " << totalParameters << std::endl;
    if(initialParameters.size() < totalParameters) {
        // If the size of the parent's mind is too low
        // Add extra splines here
        std::size_t extraParameters = totalParameters - initialParameters.size();
        
        std::cout << "Parent mind did not have enough parameters, adding " << extraParameters << " parameter vectors" << std::endl;
        
        for(std::size_t i=0;i<extraParameters;i++){
            std::vector<double> parameters(min_size, 0.5);
            
            for(std::size_t param = 0; param < min_size; param++){
                parameters[param] += sqrtvariance * _random->normal_real();
            }
            initialParameters.push_back(parameters);
        }
    }
    
    _trial = new POWER::Trial(_random, numSplines, min_size, max_size, rank_size, evaluations, variance, variance_decay, initialParameters);
	_evaluation = (_trial->policy(_ev_steps) - 0.5) * 2.0;
}

boost::ptr_vector<MindGenome> RL_PoWER::getRandomInitialMinds(){
    double sqrtvariance = std::sqrt(_globals->parameter(POWER::Trial::VARIANCE_NAME));
    std::size_t min_size = static_cast<std::size_t> (_globals->parameter(POWER::Trial::V_SIZE_MIN_NAME));
    
    std::vector<std::vector<double> > parameters(numSplines, std::vector<double>(min_size, 0.5));

    for (std::size_t spline = 0; spline < numSplines; spline++) {
        for(std::size_t param = 0; param < min_size; param++){
            parameters[spline][param] += sqrtvariance * _random->normal_real();
        }
    }

    MatrixGenome *genome = new MatrixGenome();
    genome->setValues(parameters);
    
    boost::ptr_vector<MindGenome> minds;
    minds.push_back(genome);
    return minds;
}

void RL_PoWER::reset()
{
#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Reset Parameters"
			<< std::endl;
#endif
}

void RL_PoWER::update()
{
#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Update Parameters"
			<< std::endl;
#endif

	_ev_step += 1;

	if (_ev_step >= _ev_steps) {
		_ev_step = 0;
	}
}

unsigned int RL_PoWER::getGeneration() {
    return _trial->getEvaluationIndex();
}

unsigned int RL_PoWER::getEvaluation() {
    return _trial->getEvaluationIndex();
}

bool RL_PoWER::nextEvaluation()
{
	_ev_step = 0;

	if (_trial->evolve_policy()) {
		/* Values normalization in [-1,1] */
		_evaluation = (_trial->policy(_ev_steps) - 0.5) * 2.0;

		return true;
	}

	else {
		return false;
	}
}

void RL_PoWER::setEvaluationFitness(double value)
{
#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Set Fitness" << " "
			<< value
			<< std::endl;
#endif

	value *= 100;

	_trial->policy_fitness(value);
}

void RL_PoWER::setEvaluationFitnessAlt(const std::string & string)
{
#ifdef DEBUG_ALGORITHM
	std::cout
			<< _name << ": "
			<< "Set Fitness Alternate" << " "
			<< string
			<< std::endl;
#endif

//	std::cout << string << std::endl;

	_trial->policy_fitness_alt(string);
}

void RL_PoWER::save()
{
	boost::filesystem::path path(_directory);

	std::ofstream log_fitness((path / "Fitness.txt").c_str(), std::ofstream::app);
	std::ofstream log_splines((path / "Splines.txt").c_str(), std::ofstream::app);

	_trial->dump_fitness(log_fitness);
	log_fitness.close();

	_trial->dump_splines(log_splines);
	log_splines.close();

	TiXmlDocument doc((path / (_name + ".xml")).string());

	TiXmlElement rl(_name);

	/**
	 * Not saving the flagso to evaluate again the last policy avoiding
	 * stressful program state save and loading routines in the controller.
	 */

	rl.SetAttribute("Evaluation_Steps", boost::lexical_cast<std::string>(_ev_steps));
	rl.SetAttribute("Servo_Size", boost::lexical_cast<std::string>(_evaluation.rows()));

	/**
	 * Not saving the configuration parameters because the are encapsulated within
	 * the trial XML representation
	 */
	if (!rl.InsertEndChild(_trial->save_xml())) {
		throw std::runtime_error("XML Error: Element Assembling");
	}

	if (!doc.InsertEndChild(rl)) {
		throw std::runtime_error("XML Error: Element Assembling");
	}

	if (!doc.SaveFile()) {
		throw std::runtime_error("XML Error: Saving File " + (path / (_name + ".xml")).string());
	}
}