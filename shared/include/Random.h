#ifndef RANDOM_H
#define	RANDOM_H

#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/cauchy_distribution.hpp>

#include <boost/utility.hpp>

namespace utils
{

	class Random
	:
	private boost::noncopyable
	{
	  public:

		/**
		 * Create
		 */
		Random();
		Random(unsigned int);
        
        /**
         * Destroy
         */
        ~Random();

		/**
		 * Returns the value used to seed the random
		 * @return Seed value
		 */
		unsigned int seed() const;

		/**
		 * Returns a random unsigned integer from an uniform distribution in [0, 9]
		 * @return Random unsigned integer evenly distributed
		 */
		unsigned int uniform_integer();

		/**
		 * Returns a random double from an uniform distribution in [0.0, 1.0]
		 * @return Random double evenly distributed
		 */
		double uniform_real();

		/**
		 * Returns a random double from a normal distribution with mean 0 and
		 * standard deviation 1
		 * @return Random double normally distributed
		 */
		double normal_real();

		/**
		 * Returns a random double from a Cauchy distribution with mean 0 and
		 * standard deviation 1
		 * @return Random double
		 */
		double cauchy_real();

		/**
		 * Returns a random value
		 * @return Random value
		 */
		unsigned int operator()();
        
        static Random *getInstance();

	  private:
		typedef boost::random::random_device SeedGenerator;
		typedef boost::random::mt19937 Engine;
		typedef boost::random::uniform_int_distribution<unsigned int> IntegerDist;
		typedef boost::random::uniform_real_distribution<double> RealDist;
		typedef boost::random::normal_distribution<double> NormalDist;
		typedef boost::random::cauchy_distribution<double> CauchyDist;

		SeedGenerator _seeder;

		unsigned int _seed;

		Engine _engine;
        static Random *instance;

		IntegerDist _integer_dist;
		boost::random::variate_generator<Engine, IntegerDist> _integer_gen;

		RealDist _real_dist;
		boost::random::variate_generator<Engine, RealDist> _real_gen;

		NormalDist _normal_dist;
		boost::random::variate_generator<Engine, NormalDist> _normal_gen;

		CauchyDist _cauchy_dist;
		boost::random::variate_generator<Engine, CauchyDist> _cauchy_gen;
	};
}
#endif	/* RANDOM_H */