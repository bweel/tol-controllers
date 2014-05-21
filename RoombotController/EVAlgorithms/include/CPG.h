#ifndef CPG_H
#define	CPG_H

#include "EVAlgorithm.h"

#include "Vector_3.h"

#include <codyn/codyn.h>
#include <codyn/cdn-integrators.h>
#include <codyn/integrators/cdn-integrator.h>
#include <codyn/integrators/cdn-integrator-euler.h>
#include <codyn/integrators/cdn-integrator-predict-correct.h>
#include <codyn/integrators/cdn-integrator-runge-kutta.h>

#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <string>
#include <vector>

template <class T>
class CPG
:
virtual public EVAlgorithm<transforms::Vector_3,T>
{
	public:
		CPG(const std::string &, const std::string &, const std::string &, const double &);
		
		void set_parameter(const transforms::Vector_3 &, const T &);
		void set_parameter_extra(const int &, const T &) {};
		
		T get_parameter(const transforms::Vector_3 &);
		
		void reset();
		void update();
		
		void next_evaluation();
		void set_evaluation_fitness(const double &);
		void set_evaluation_fitness_alt(const std::string &);
		void set_evaluation_reward(const double &);
		
		void save(const std::string &);
		void load(const std::string &);
	private:
		double _time;
		double _time_step;
		
		GSList * _integrators;
		CdnIntegrator * _integrator;
		CdnNetwork * _network;
		
		std::vector<double> _fitness;
		std::vector<std::string> _fitness_alt;
};

template <class T>
CPG<T>::CPG(const std::string & name, const std::string & p_path, const std::string & d_path, const double & time_step)
:
EVAlgorithm<transforms::Vector_3,T>::EVAlgorithm(name),
_time(0.0),
_time_step(time_step),
_integrator(NULL),
_network(NULL)
{
	guint seed;
	boost::random::random_device rng;
	boost::random::uniform_int_distribution<guint> dist(0, UINT_MAX);
	GError * error = NULL;
	
	seed = dist(rng);
	
	//_integrators = cdn_integrators_create();
	/*
	 Select Integrator
	 * switch(_i_type)
	 * {
	 * case 1:
	 * _integrator = cdn_integrator_euler_new();
	 * break;
	 * case 2:
	 * _integrator = cdn_integrator_predict_correct_new();
	 * case 3:
	 * _integrator = cdn_integrator_runge_kutta_new();
	 * 
	 * 
	 * }
	 * 
	 * if(! _integrator) {
	 *	throw std::runtime_error("");
	 * }
	 */
	//_integrator = cdn_integrator_
	/* Load The File */
	
	/* Setup Network */
	
	_network = cdn_network_new_from_path(p_path.data(), & error);
	
	if (! _network) {
		throw std::runtime_error("");
		// Read Error & Throw
	}
	
	cdn_network_set_random_seed(_network, seed);
	/* Eliminate Tree Similarities - From Supervisor */
	cdn_network_set_integrator(_network, _integrator);
	
	// Setup Integrator
}

template <class T>
void CPG<T>::set_parameter(const transforms::Vector_3 & index, const T & value)
{
	/* Search Node */
		/* If Not Exists Throw */
	/* Else Set Value */
}

template <class T>
T CPG<T>::get_parameter(const transforms::Vector_3 & index)
{
	
}

template <class T>
void CPG<T>::reset()
{
	
}

template <class T>
void CPG<T>::update()
{
	cdn_network_step(_network, _time_step);
}

template <class T>
void CPG<T>::next_evaluation()
{
	
}

template <class T>
void CPG<T>::set_evaluation_fitness(const double & fitness)
{
	
}

template <class T>
void CPG<T>::set_evaluation_fitness_alt(const std::string & fitness_alt)
{
	
}

template <class T>
void CPG<T>::set_evaluation_reward(const double & reward)
{
	
}

template <class T>
void CPG<T>::save(const std::string & path)
{
	
}

template <class T>
void CPG<T>::load(const std::string & path)
{
	
}
#endif	/* CPG_H */