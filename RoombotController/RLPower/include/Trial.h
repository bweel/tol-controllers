#ifndef TRIAL_H
#define	TRIAL_H

#include "Globals.h"
#include "Interval.h"
#include "Policy.h"
#include "Values.h"
#include "Logger.h"

#include "Random.h"

#include "tinyxmlplus.h"

#include <iostream>
#include <vector>

namespace POWER
{

	class Trial
	{
	  public:

		static const double VARIANCE;
		static const double VARIANCE_DECAY;

		static const std::size_t V_SIZE_MIN;
		static const std::size_t V_SIZE_MAX;
		static const std::size_t RANKING_SIZE;
		static const std::size_t EVALUATION_SIZE;

		static const std::string VARIANCE_NAME;
		static const std::string VARIANCE_DECAY_NAME;

		static const std::string V_SIZE_MIN_NAME;
		static const std::string V_SIZE_MAX_NAME;
		static const std::string RANKING_SIZE_NAME;
		static const std::string EVALUATION_SIZE_NAME;
        
        static const double INTERVAL_START;
		static const double INTERVAL_END;

		static const std::string XML_NAME;

		// <editor-fold defaultstate="collapsed" desc="Constructors">

		/**
		 * Starts a new trial
		 */
		Trial(Utils::Random *random, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, double, double, std::vector<std::vector<double> > parameters, bool = false);

		/**
		 *
		 * @param
		 */
		Trial(const TiXmlElement &);

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Destructor">

		virtual ~Trial();

		// </editor-fold>

		/**
		 * Returns the current policy values for evaluation
		 * @param Number of steps for one servo complete cycle
		 * @return Current policy values
		 */
		Values policy(std::size_t);

		/**
		 * Returns the best policy values for evaluation
		 * @param Number of steps for one servo complete cycle
		 * @return Best policy values
		 */
		Values best_policy(std::size_t);

		/**
		 * Sets the current policy fitness
		 * @param Fitness value
		 */
		void policy_fitness(double);

		/**
		 * Sets the current policy alternate fitness data
		 * @param Alternate fitness data
		 */
		void policy_fitness_alt(const std::string &);

		/**
		 * Evolves the current evaluated policy to the next
		 * @return True Success, False Evaluations Completed
		 */
		bool evolve_policy();

		void dump_fitness(std::ostream &);

		void dump_splines(std::ostream &);

		/**
		 * Save class internal data in XML format
		 * @return XML Element representing class
		 */
		TiXmlElement save_xml() const;
        
        std::size_t getEvaluationIndex();

        
	  private:
        log4cpp::Category &logger;
        
		static const double FITNESS_EXP;
		static const double EPSILON;

		typedef std::vector<Policy *>::iterator PIterator;
		typedef std::vector<Policy *>::const_iterator PConstIterator;

		Utils::Random * _random;

		std::size_t _v_size_min;
		std::size_t _v_size_max;
		Interval _interval;

		std::size_t _rank_size;

		std::size_t _ev_size;
		std::size_t _ev_delta;
		std::size_t _ev_index;

		double _variance_decay;
		std::vector<Policy *> _ranking;
		std::vector<Policy *> _evaluations;
        
        Values random_normal_distribution(std::size_t, std::size_t);
		Values random_cauchy_distribution(std::size_t, std::size_t);
	};
}
#endif	/* TRIAL_H */