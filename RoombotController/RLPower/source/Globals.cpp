#include "Globals.h"
#include "Trial.h"

#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <stdexcept>

namespace POWER
{

	const std::string Globals::XML_NAME = "Globals";
	Globals * Globals::_instance = 0;

	Globals * Globals::init()
	{
		if (_instance) {
			delete _instance;
		}

		_instance = new Globals();

        _instance->logger.debug("RL_PoWER: Globals Init");

		return _instance;
	}

	Globals * Globals::load(const std::string & file_path)
	{
		if (_instance) {
			delete _instance;
		}

		_instance = new Globals(file_path);

        _instance->logger.debug("RL_PoWER: Globals Loaded");

		return _instance;
	}

	Globals * Globals::instance()
	{
		if (!_instance) {
			throw std::runtime_error("Globals Not Initialized");
		}

		return _instance;
	}

	void Globals::save(const std::string & file_path)
	{
		if (!_instance) {
			throw std::runtime_error("Globals Not Initialized");
		}

		TiXmlDocument g_doc(file_path);

		if (!g_doc.InsertEndChild(_instance->save_xml())) {
			throw std::runtime_error("XML Error: Element Assembling");
		}

		if (!g_doc.SaveFile()) {
			throw std::runtime_error("XML Error: Saving File " + file_path);
		}

		std::cout << "RL_PoWER: Globals Saved" << std::endl;
	}

	void Globals::deinit()
	{
		if (_instance) {
			delete(_instance);
		}
	}

	/*unsigned int Globals::seed() const
	{
		return _random->seed();
	}*/

	double Globals::parameter(const std::string & name) const
	{
		return _tuples.at(name);
	}

	/*void Globals::seed(unsigned int seed)
	{
		std::cout << "Seeding With: " << seed << std::endl;

		if (_random) {
			delete(_random);
		}

		_random = new utils::Random(seed);
	}*/

	void Globals::parameter(const std::string & name, double value)
	{
		_tuples[name] = value;
	}

	/*utils::Random & Globals::random()
	{
		return (* _random);
	}*/

	TiXmlElement Globals::save_xml() const
	{
		TiXmlElement result(Globals::XML_NAME);

		result.SetAttribute("Size", boost::lexical_cast<std::string>(_tuples.size()));

		for (MConstIterator it = _tuples.begin(); it != _tuples.end(); it++) {
			TiXmlElement tuple("Tuple");

			tuple.SetAttribute("Name", it->first);
			tuple.SetDoubleAttribute("Value", it->second);

			if (!result.InsertEndChild(tuple)) {
				throw std::runtime_error("XML Error: Element Assembling");
			}
		}

		return result;
	}

	Globals::Globals()
	:
	_tuples(_init_parameters()),
    logger(Logger::getInstance("RLPower"))
    {
    }

	Globals::Globals(const std::string & file_path)
	:
	_tuples(),
    logger(Logger::getInstance("RLPower"))
	{
		TiXmlDocument doc(file_path);

		if (!doc.LoadFile()) {
			throw std::runtime_error("XML Error: Loading File " + file_path);
		}

		const TiXmlElement * globals = doc.FirstChildElement(Globals::XML_NAME);

		if (!globals) {
			throw std::runtime_error("XML Error: Element Parsing");
		}

		std::size_t size;

		if (globals->QueryValueAttribute<std::size_t>("Size", &size)) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		for (const TiXmlElement * child = globals->FirstChildElement("Tuple"); child; child = child->NextSiblingElement("Tuple")) {
			const char * data = child->Attribute("Name");

			if (!data) {
				throw std::runtime_error("XML Error: Attribute Parsing");
			}

			double value;
			std::string name = data;

			if (child->QueryDoubleAttribute("Value", &value)) {
				throw std::runtime_error("XML Error: Attribute Parsing");
			}

			_tuples[name] = value;
		}

		if (_tuples.size() != size) {
			throw std::runtime_error("XML Error: Element Parsing");
		}

		//_random = new utils::Random();
	}

	Globals::~Globals() {
		/*if (_random) {
			delete(_random);
		}*/ }

	std::map<std::string, double> Globals::_init_parameters()
	{
		std::map<std::string, double> result;

		result[Trial::V_SIZE_MIN_NAME] = Trial::V_SIZE_MIN;
		result[Trial::V_SIZE_MAX_NAME] = Trial::V_SIZE_MAX;
		result[Trial::VARIANCE_NAME] = Trial::VARIANCE;
		result[Trial::VARIANCE_DECAY_NAME] = Trial::VARIANCE_DECAY;
		result[Trial::RANKING_SIZE_NAME] = Trial::RANKING_SIZE;
		result[Trial::EVALUATION_SIZE_NAME] = Trial::EVALUATION_SIZE;

		return result;
	}
}