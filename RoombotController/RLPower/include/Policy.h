#ifndef POLICY_H
#define	POLICY_H

#include "Spline.h"
#include "Values.h"

#include <string>
#include <valarray>
#include <vector>

namespace POWER
{

	class Policy
	{
	  public:

		/**
		 * Class name for XML
		 */
		static const std::string XML_NAME;

		// <editor-fold defaultstate="collapsed" desc="Comparison Functors">

		struct less
		{

			inline bool operator()(const Policy * lhs, const Policy * rhs)
			{
				return ((* lhs) < (* rhs));
			}
		};

		struct less_equal
		{

			inline bool operator()(const Policy * lhs, const Policy * rhs)
			{
				return ((* lhs) <= (* rhs));
			}
		};

		struct greater
		{

			inline bool operator()(const Policy * lhs, const Policy * rhs)
			{
				return ((* lhs) > (* rhs));
			}
		};

		struct greater_equal
		{

			inline bool operator()(const Policy * lhs, const Policy * rhs)
			{
				return ((* lhs) >= (* rhs));
			}
		};

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Constructors">

		Policy(std::size_t, double, const Interval &, const Values &);
		Policy(const TiXmlElement &);

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Destructor">

		virtual ~Policy();

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Accessors">

		/**
		 * Returns the policy evaluation status
		 * @return Policy evaluation status
		 */
		bool is_evaluated() const;

		/**
		 * Returns the index associated to the policy
		 * @return Policy index
		 */
		std::size_t index() const;

		/**
		 * Returns the variance associated to the policy
		 * @return Policy variance
		 */
		double variance() const;

		/**
		 * Returns the fitness value associated with the policy
		 * @return Fitness value of the policy
		 */
		double fitness() const;

		/**
		 * Returns the data associated with the policy
		 * @return Data associated with the policy
		 */
		std::string data() const;

		/**
		 *
		 * @param
		 * @return
		 */
		Values parameters(const Interval &, std::size_t = 0);

		Values splines(const Interval &);

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Mutators">

		/**
		 * Assigns a fitness value to the policy
		 * @param Fitness value
		 */
		void fitness(double);

		/**
		 * Assigns
		 * @param
		 */
		void data(const std::string &);

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Operators">

		friend bool operator<(const Policy &, const Policy &);
		friend bool operator>(const Policy &, const Policy &);
		friend bool operator<=(const Policy &, const Policy &);
		friend bool operator>=(const Policy &, const Policy &);

		// </editor-fold>

		/**
		 * Save class internal data in XML format
		 * @return XML Element representing class
		 */
		TiXmlElement save_xml() const;
        
        
        std::vector<Spline *> getSplines();

	  private:

		typedef std::vector<Spline *>::iterator SIterator;
		typedef std::vector<Spline *>::const_iterator SConstIterator;

		bool _is_evaluated;

		std::size_t _index;

		double _variance;

		double _fitness;
		std::string _data;

		std::vector<Spline *> _splines;

		std::vector<Spline *> _init_splines(const Interval &, const Values &);
	};
}

#endif	/* POLICY_H */