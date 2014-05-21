#include "EVAlgorithm.h"

EVAlgorithm::EVAlgorithm(const std::string & name)
:
_name(name) { }

EVAlgorithm::~EVAlgorithm() { }

std::string EVAlgorithm::getName() const
{
	return _name;
}