#include "LearningAlgorithm.h"

LearningAlgorithm::LearningAlgorithm(const std::string & name)
:
_name(name) { }

LearningAlgorithm::~LearningAlgorithm() { }

std::string LearningAlgorithm::getName() const
{
	return _name;
}