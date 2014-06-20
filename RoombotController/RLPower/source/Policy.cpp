#include "Policy.h"

#include <boost/lexical_cast.hpp>

#include <stdexcept>

namespace POWER
{
	const std::string Policy::XML_NAME = "Policy";

	// <editor-fold defaultstate="collapsed" desc="Constructors">

	Policy::Policy(std::size_t index, double variance, const Interval & interval, const Values & values)
	:
	_is_evaluated(false),
	_index(index),
	_variance(variance),
	_fitness(0.0),
	_data(""),
	_splines(_init_splines(interval, values)) { }

	Policy::Policy(const TiXmlElement & element)
	{
		if (Policy::XML_NAME != element.ValueStr()) {
			throw std::invalid_argument("Element Does Not Represent Class");
		}

		if (element.QueryValueAttribute<std::size_t>("Index", &_index)) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		if (element.QueryValueAttribute<bool>("Evaluated", &_is_evaluated)) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		if (element.QueryDoubleAttribute("Variance", &_variance)) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		if (element.QueryDoubleAttribute("Fitness", &_fitness)) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		const char * data = element.Attribute("Data");

		if (!data) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		_data = data;

		std::size_t size;

		if (element.QueryValueAttribute("Splines", &size)) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		_splines.reserve(size);

		for (const TiXmlElement * child = element.FirstChildElement("Spline"); child; child = child->NextSiblingElement("Spline")) {
			_splines.push_back(new Spline(* child));
		}

		if (_splines.size() != size) {
			throw std::runtime_error("XML Error: Element Parsing");
		}
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Destructor">

	Policy::~Policy()
	{
		for (SIterator it = _splines.begin(); it != _splines.end(); it++) {
			if (* it) {
				delete(* it);
			}
		}
	}

	// </editor-fold>

	bool Policy::is_evaluated() const
	{
		return _is_evaluated;
	}

	double Policy::variance()const
	{
		return _variance;
	}

	std::size_t Policy::index() const
	{
		return _index;
	}

	double Policy::fitness() const
	{
		return _fitness;
	}

	std::string Policy::data() const
	{
		return _data;
	}

	Values Policy::parameters(const Interval & interval, std::size_t size)
	{
		std::size_t rows(_splines.size());
		std::size_t columns((size) ? size : interval.size());

		std::valarray<double> x(interval.x(columns + 1)[std::slice(0, columns, 1)]);
		Values result(rows, columns);

		for (std::size_t index = 0; index < rows; index++) {
			result.row(index, _splines[index]->y(x));
		}

		return result;
	}

	Values Policy::splines(const Interval & interval)
	{
		std::size_t size = interval.size() + 1;
		std::size_t rows(size);
		std::size_t columns(_splines.size());

		std::valarray<double> x(interval.x(size));

		Values result(rows, columns);

		for (std::size_t index = 0; index < columns; index++) {
			result.column(index, _splines[index]->y(x));
		}

		return result;
	}

	void Policy::fitness(double value)
	{
		_is_evaluated = true;
		_fitness = value;
	}

	void Policy::data(const std::string & data)
	{
		_data = data;
	}

	std::vector<Spline *> Policy::_init_splines(const Interval & interval, const Values & values)
	{
		std::size_t rows(values.rows());
		std::size_t columns(values.columns());

		std::valarray<double> x_row(interval.x(columns + 1));
		std::valarray<double> y_row(columns + 1);

		std::vector<Spline *> result(rows);

		for (std::size_t index = 0; index < rows; index++) {
			y_row[std::slice(0, columns, 1)] = values.row(index);
			y_row[columns] = y_row[0];

			_splines[index] = new Spline(x_row, y_row);
		}

		return result;
	}

	bool operator<(const Policy & lhs, const Policy & rhs)
	{
		return (lhs._fitness < rhs._fitness);
	}

	bool operator>(const Policy & lhs, const Policy & rhs)
	{
		return (lhs._fitness > rhs._fitness);
	}

	bool operator<=(const Policy & lhs, const Policy & rhs)
	{
		return !(lhs > rhs);
	}

	bool operator>=(const Policy & lhs, const Policy & rhs)
	{
		return !(lhs < rhs);
	}

	TiXmlElement Policy::save_xml() const
	{
		TiXmlElement result(Policy::XML_NAME);

		result.SetAttribute("Index", boost::lexical_cast<std::string>(_index));
		result.SetAttribute("Evaluated", boost::lexical_cast<std::string>(_is_evaluated));
		result.SetDoubleAttribute("Variance", _variance);
		result.SetDoubleAttribute("Fitness", _fitness);
		result.SetAttribute("Data", _data);

		result.SetAttribute("Splines", boost::lexical_cast<std::string>(_splines.size()));

		for (SConstIterator it = _splines.begin(); it != _splines.end(); it++) {
			if (!result.InsertEndChild((* it)->save_xml())) {
				throw std::runtime_error("XML Error: Element Assembling");
			}
		}

		return result;
	}
}