#include "Trial.h"

#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <complex>

namespace POWER
{
	const double Trial::VARIANCE = 0.008;
	const double Trial::VARIANCE_DECAY = 0.98;

	const std::size_t Trial::V_SIZE_MIN = 2;
	const std::size_t Trial::V_SIZE_MAX = 20;
	const std::size_t Trial::RANKING_SIZE = 10;
	const std::size_t Trial::EVALUATION_SIZE = 1000;

	const std::string Trial::VARIANCE_NAME = "Variance";
	const std::string Trial::VARIANCE_DECAY_NAME = "Variance Decay";
	const std::string Trial::V_SIZE_MIN_NAME = "Values Size Min";
	const std::string Trial::V_SIZE_MAX_NAME = "Values Size Max";
	const std::string Trial::RANKING_SIZE_NAME = "Ranking Size";
	const std::string Trial::EVALUATION_SIZE_NAME = "Evaluation Size";

	const std::string Trial::XML_NAME = "Trial";

	const double Trial::INTERVAL_START = 0.0;
	const double Trial::INTERVAL_END = 1.0;
	const double Trial::FITNESS_EXP = 6.0;
	const double Trial::EPSILON = 1e-10;

	Trial::Trial(utils::Random *random,
				std::size_t numSplines,
				std::size_t min_size,
				std::size_t max_size,
				std::size_t rank_size,
				std::size_t ev_size,
				double variance,
				double variance_decay,
                std::vector<std::vector<double> > parameters,
				bool adaptive)
	:
	_random(random),
	_v_size_min((Trial::V_SIZE_MIN <= min_size) ? min_size : throw std::invalid_argument("Interval minimum length should be larger than "+Trial::V_SIZE_MIN)),
	_v_size_max((min_size <= max_size) ? max_size : throw std::invalid_argument("Interval maximum length should be larger than the minimum length")),
	_interval(Trial::INTERVAL_START, Trial::INTERVAL_END, min_size),
	_rank_size((0 < rank_size) ? rank_size : throw std::invalid_argument("Rank size should be positive")),
	_ev_size((ev_size > (_v_size_max - _v_size_min)) ? ev_size : throw std::invalid_argument("Evaluation size should be larger than the interval maximum length")),
	_ev_delta((_v_size_max - _v_size_min) ? static_cast<std::size_t> (round(_ev_size / (_v_size_max - _v_size_min))) : 1),
	//_ev_delta((_v_size_max - _v_size_min) ? static_cast<std::size_t> (round(_ev_size / ((_v_size_max - _v_size_min) / 2.0))) : 1),
	_ev_index(0),
	_variance_decay((0.0 < variance_decay) ? variance_decay : throw std::invalid_argument("Variance Decay should be positive"))
	{
		if (0.0 >= variance) {
			throw std::invalid_argument("Variance must be higher than 0");
		}

		if (!numSplines) {
			throw std::invalid_argument("The number of splines needs to be greater than 0");
		}

		_ranking.reserve(_rank_size);
		_evaluations.reserve(_ev_size);
        
        Values values(parameters.size(),parameters[0].size());
        
        for(int i=0;i<parameters.size();i++){
            for(int j=0;j<parameters[0].size();j++){
                values.row(i)[j] = parameters[i][j];
            }
        }

		_evaluations.push_back(new Policy(_ev_index, variance, _interval, values));

		std::cout
				<< "RL_PoWER: Starting Trial" << std::endl
				<< "Variance: " << variance << " Decay Factor: " << _variance_decay << std::endl
				<< "Interval: [" << _interval.min() << "," << _interval.max() << "]" << std::endl
				<< "Parameters # Start: " << _v_size_min << std::endl
				<< "Parameters # End: " << _v_size_max << std::endl
				<< "Parameters # Update: " << "Each " << _ev_delta << " Evaluations" << std::endl
				<< "Evaluations #: " << _ev_size << std::endl;
	}

	Trial::Trial(const TiXmlElement & element)
	:
	_interval(Trial::INTERVAL_START, Trial::INTERVAL_END)
	{
		if (Trial::XML_NAME != element.ValueStr()) {
			throw std::invalid_argument("Element Does Not Represent Class");
		}

		unsigned int seed = 0;

		if (element.QueryValueAttribute<unsigned int>("Seed", &seed)) {
			throw std::invalid_argument("XML Error: Attribute Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
		}

		if (element.QueryValueAttribute<std::size_t>("ValuesStartSize", &_v_size_min)) {
			throw std::invalid_argument("XML Error: Attribute Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
		}

		if (element.QueryValueAttribute<std::size_t>("ValuesEndSize", &_v_size_max)) {
			throw std::invalid_argument("XML Error: Attribute Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
		}

		if (element.QueryDoubleAttribute("VarianceDecay", &_variance_decay)) {
			throw std::invalid_argument("XML Error: Attribute Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
		}

		const TiXmlElement * interval = element.FirstChildElement("Interval");
		const TiXmlElement * ranking = element.FirstChildElement("Ranking");
		const TiXmlElement * evaluations = element.FirstChildElement("Evaluations");

		if ((!interval) || (!ranking) || (!evaluations)) {
			throw std::invalid_argument("XML Error: Element Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
		}

		_interval = Interval(* interval);

		if (evaluations->QueryValueAttribute<std::size_t>("Index", &_ev_index)) {
			throw std::invalid_argument("XML Error: Attribute Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
		}

		if (evaluations->QueryValueAttribute<std::size_t>("Delta", &_ev_delta)) {
			throw std::invalid_argument("XML Error: Attribute Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
		}

		if (evaluations->QueryValueAttribute<std::size_t>("Size", &_ev_size)) {
			throw std::invalid_argument("XML Error: Attribute Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
		}

		std::size_t index = 0;
		std::size_t size = _ev_index + 1;

		_evaluations.reserve(_ev_size);

		for (const TiXmlElement * child = evaluations->FirstChildElement("Policy"); child; child = child->NextSiblingElement("Policy")) {
			_evaluations.push_back(new Policy(* child));

			index += 1;
		}

		if (index != size) {
			throw std::invalid_argument("XML Error: Element Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
		}

		index = 0;

		if (ranking->QueryValueAttribute<std::size_t>("Size", &size)) {
			throw std::invalid_argument("XML Error: Attribute Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
		}

		if (ranking->QueryValueAttribute<std::size_t>("SizeMax", &_rank_size)) {
			throw std::invalid_argument("XML Error: Attribute Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
		}

		_ranking.reserve(_rank_size);

		for (const TiXmlElement * child = ranking->FirstChildElement("Element"); child; child = child->NextSiblingElement("Element")) {
			std::size_t p_index;

			if (child->QueryValueAttribute<std::size_t>("Index", &p_index)) {
				throw std::invalid_argument("XML Error: Attribute Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
			}

			_ranking.push_back(_evaluations[p_index]);

			index += 1;
		}

		if (index != size) {
			throw std::invalid_argument("XML Error: Element Parsing" + boost::lexical_cast<std::string>(__FILE__) + " " + boost::lexical_cast<std::string>(__LINE__));
		}

		_random = new utils::Random(seed);
	}

	Trial::~Trial()
	{
		if (_random) {
			delete (_random);
		}

		for (PIterator it = _evaluations.begin(); it != _evaluations.end(); it++) {
			if (*it) {
				delete(* it);
			}
		}
	}
    
    std::size_t Trial::getEvaluationIndex() {
        return _ev_index;
        
    }

	Values Trial::policy(std::size_t steps)
	{
		return _evaluations[_ev_index]->parameters(_interval, steps);
	}

	Values Trial::best_policy(std::size_t steps)
	{
		return _ranking[0]->parameters(_interval, steps);
	}

	void Trial::policy_fitness(double value)
	{
		if (_ev_index > _ev_size) {
			return;
		}

		Policy * policy = _evaluations[_ev_index];

		/*
		 * We calculate the modified fitness here to avoid calculating it every
		 * time when we evolve a policy
		 */
		policy->fitness(std::pow(value, Trial::FITNESS_EXP));

		/*
		 * We maintain a ranking of the n best performing policies in descending
		 * order
		 */
		PIterator it = std::upper_bound(
										_ranking.begin(),
										_ranking.end(),
										policy,
										Policy::greater()
										);

		_ranking.insert(it, policy);

		/*
		 * Deleting the n+1-th element of the ranking, there should be n at most
		 */
		if (_ranking.size() > _rank_size) {
			_ranking.resize(_rank_size);
		}
	}

	void Trial::policy_fitness_alt(const std::string & data)
	{
		if (_ev_index > _ev_size) {
			return;
		}

		_evaluations[_ev_index]->data(data);
	}

	bool Trial::evolve_policy()
	{
		if (_ev_index >= _ev_size) {
			return false;
		}

		Policy * policy = _evaluations[_ev_index];

		if (!policy->is_evaluated()) {
			throw std::runtime_error("Policy Not Evaluated");
		}

		std::size_t v_size = _interval.size();

		if ((v_size < _v_size_max) && (!(_ev_index % _ev_delta))) {
			//std::cout << _ev_index << " " << v_size << " " << (_ev_index % _ev_delta) << std::endl;
			_interval.resize(v_size + 1);
			//_interval.resize(static_cast<std::size_t> (v_size * 2.0));
		}

		double variance = policy->variance();
		Values parameters = policy->parameters(_interval);

		double denominator(0.0);
		Values numerator(parameters.rows(), parameters.columns());
		Values exploration(parameters.rows(), parameters.columns());

		for (PIterator it = _ranking.begin(); it != _ranking.end(); it++) {
			/*
			 * The fitness is already adjusted when it is set to avoid multiple
			 * recalculations
			 */
			double f_champion = (* it)->fitness();

			/* Difference Between Champion Parameters And Current Ones */
			exploration = (* it)->parameters(_interval) - parameters;

			/* Weighting Exploration Against Returns */
			numerator += exploration * f_champion;
			denominator += f_champion;
		}

		variance *= _variance_decay;

		//std::cout << "Input Parameters: " << values << std::endl;
		//std::cout << "Numerator: " << numerator << std::endl;
		//std::cout << "Denominator: " << denominator << std::endl;
		//std::cout << "Variance: " << _variance << std::endl;
		//std::cout << "Standard Deviation: " << std::sqrt(_variance) << std::endl;

		parameters += (numerator / (denominator + Trial::EPSILON));

		//std::cout << "Output Parameters 1: " << values << std::endl;

		parameters += (std::sqrt(variance) * random_normal_distribution(parameters.rows(), parameters.columns()));
		//parameters += (std::sqrt(variance) * random_cauchy_distribution(parameters.rows(), parameters.columns()));

		//std::cout << "Output Parameters 2: " << values << std::endl;

		_evaluations.push_back(new Policy(_ev_index + 1, variance, _interval, parameters));
		_ev_index += 1;

		return true;
	}

	void Trial::dump_fitness(std::ostream & out)
	{
		for (PConstIterator it = _evaluations.begin(); it != _evaluations.end(); it++) {
			if (* it) {

				out << (*it)->index() << " " << (*it)->data() << std::endl;
			}
		}
	}

	void Trial::dump_splines(std::ostream & out)
	{
		for (PConstIterator it = _evaluations.begin(); it != _evaluations.end(); it++) {
			if (* it) {

				out << "Policy: " << (*it)->index() << std::endl;
				out << (* it)->splines(_interval) << std::endl;
			}
		}
	}

	TiXmlElement Trial::save_xml() const
	{
		TiXmlElement result(Trial::XML_NAME);
		TiXmlElement ranking("Ranking");
		TiXmlElement evaluations("Evaluations");

		result.SetAttribute("Seed", boost::lexical_cast<std::string>(_random->seed()));

		result.SetAttribute("StartValuesSize", boost::lexical_cast<std::string>(_v_size_min));
		result.SetAttribute("EndValuesSize", boost::lexical_cast<std::string>(_v_size_max));

		result.SetDoubleAttribute("VarianceDecay", _variance_decay);

		ranking.SetAttribute("Size", boost::lexical_cast<std::string>(_ranking.size()));
		ranking.SetAttribute("SizeMax", boost::lexical_cast<std::string>(_rank_size));

		evaluations.SetAttribute("Index", boost::lexical_cast<std::string>(_ev_index));
		evaluations.SetAttribute("Delta", boost::lexical_cast<std::string>(_ev_delta));
		evaluations.SetAttribute("Size", boost::lexical_cast<std::string>(_ev_size));

		if (!result.InsertEndChild(_interval.save_xml())) {
			throw std::runtime_error("XML Error: Element Assembling");
		}

		for (PConstIterator it = _evaluations.begin(); it != _evaluations.end(); it++) {
			if (!evaluations.InsertEndChild((* it)->save_xml())) {
				throw std::runtime_error("XML Error: Element Assembling");
			}
		}

		for (PConstIterator it = _ranking.begin(); it != _ranking.end(); it++) {
			TiXmlElement element("Element");

			element.SetAttribute("Index", boost::lexical_cast<std::string>((* it)->index()));

			if (!ranking.InsertEndChild(element)) {
				throw std::runtime_error("XML Error: Element Assembling");
			}
		}

		if (!result.InsertEndChild(evaluations)) {
			throw std::runtime_error("XML Error: Element Assembling");
		}

		if (!result.InsertEndChild(ranking)) {

			throw std::runtime_error("XML Error: Element Assembling");
		}

		return result;
	}

	Values Trial::random_normal_distribution(std::size_t rows, std::size_t columns)
	{
		Values result(rows, columns);
		std::size_t size = result.size();

		for (std::size_t index = 0; index < size; index++) {

			result[index] = _random->normal_real();
		}

		return result;
	}

	Values Trial::random_cauchy_distribution(std::size_t rows, std::size_t columns)
	{
		Values result(rows, columns);
		std::size_t size = result.size();

		for (std::size_t index = 0; index < size; index++) {
			result[index] = _random->cauchy_real();
		}

		return result;
	}
}