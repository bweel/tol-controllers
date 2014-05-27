#include "Interval.h"

#include <boost/lexical_cast.hpp>

#include <limits>
#include <stdexcept>

namespace POWER
{
	const std::size_t Interval::I_SIZE_MIN = 2;
	const std::size_t Interval::I_SIZE_MAX = std::numeric_limits<std::size_t>::max();
	const std::string Interval::XML_NAME = "Interval";

	Interval::Interval(double start, const double end, std::size_t size)
	:
	_start(start),
	_end(end),
	_length((0.0 >= (_end - _start)) ? throw std::invalid_argument("Start Point >= End Point") : (_end - _start)),
	_delta((Interval::I_SIZE_MIN > size) ? throw std::domain_error("Size < Minimum Size") : (_length / static_cast<double> (size - 1))),
	_x(size)
	{
		_x[0] = _start;

		for (std::size_t index = 1; index < (size - 1); index++) {
			_x[index] = _start + (_delta * static_cast<double> (index));
		}

		_x[size - 1] = _end;
	}

	Interval::Interval(const TiXmlElement & element)
	{
		if (Interval::XML_NAME != element.ValueStr()) {
			throw std::invalid_argument("Element Does Not Represent Class");
		}

		if (element.QueryDoubleAttribute("Start", &_start)) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		if (element.QueryDoubleAttribute("End", &_end)) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		_length = (_end - _start);

		if (0.0 >= _length) {
			throw std::runtime_error("Start Point >= End Point");
		}

		std::size_t size = 0;

		if (element.QueryValueAttribute<std::size_t>("Size", &size)) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		if (Interval::I_SIZE_MIN > size) {
			throw std::domain_error("Size < Minimum Size");
		}

		_x.resize(size);
		_delta = _length / static_cast<double> (size - 1);

		_x[0] = _start;

		for (std::size_t index = 1; index < (size - 1); index++) {
			_x[index] = _start + (_delta * static_cast<double> (index));
		}

		_x[size - 1] = _end;
	}

	std::valarray<double> Interval::x(std::size_t size) const
	{
		if (Interval::I_SIZE_MIN > size) {
			throw std::domain_error("Size < Minimum Size");
		}

		double delta = (_length / static_cast<double> (size - 1));
		std::valarray<double> result(size);

		result[0] = _start;

		for (std::size_t index = 1; index < (size - 1); index++) {
			result[index] = _start + (delta * static_cast<double> (index));
		}

		result[size - 1] = _end;

		return result;
	}

	void Interval::resize(std::size_t size)
	{
		if (Interval::I_SIZE_MIN > size) {
			throw std::domain_error("Size < Minimum Size");
		}

		_delta = (_length / static_cast<double> (size - 1));
		_x = std::valarray<double>(size);

		_x[0] = _start;

		for (std::size_t index = 1; index < (size - 1); index++) {
			_x[index] = _start + (_delta * static_cast<double> (index));
		}

		_x[size - 1] = _end;
	}

	const double & Interval::operator [](std::size_t index) const {
		if (index >= _x.size()) {
			throw std::out_of_range("Index Out Of Range");
		}

		return _x[index];
	}

	TiXmlElement Interval::save_xml() const
	{
		TiXmlElement result(Interval::XML_NAME);

		result.SetDoubleAttribute("Start", _start);
		result.SetDoubleAttribute("End", _end);
		result.SetAttribute("Size", boost::lexical_cast<std::string>(_x.size()));

		return result;
	}
}
