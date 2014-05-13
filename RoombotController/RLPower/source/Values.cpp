#include "Values.h"

#include <boost/lexical_cast.hpp>

#include <limits>
#include <stdexcept>

namespace POWER
{

	const std::string Values::XML_NAME = "Values";

	Values::Values(std::size_t rows, std::size_t columns, double value)
	:
	_rows(rows ? rows : throw std::domain_error("Row Size Can't Be Zero")),
	_columns(columns ? columns : throw std::domain_error("Column Size Can't Be Zero"))
	{
		if (_columns > (std::numeric_limits<std::size_t>::max() / _rows)) {
			throw std::overflow_error("");
		}

		_values = std::valarray<double>(value, (_rows * _columns));
	}

	Values::Values(std::size_t rows, const std::valarray<double> & values)
	:
	_rows(rows ? rows : throw std::domain_error("Row Size Can't Be Zero")),
	_columns(values.size() ? values.size() : throw std::domain_error("Column Size Can't Be Zero")),
	_values(_init_columns(values)) { }

	Values::Values(const std::valarray<double> & values, std::size_t columns)
	:
	_rows(values.size() ? values.size() : throw std::domain_error("Row Size Can't Be Zero")),
	_columns(columns ? columns : throw std::domain_error("Column Size Can't Be Zero")),
	_values(_init_rows(values)) { }

	Values::Values(std::size_t rows, std::size_t columns, const std::valarray<double> & values)
	:
	_rows(rows ? rows : throw std::domain_error("Row Size Can't Be Zero")),
	_columns(columns ? columns : throw std::domain_error("Column Size Can't Be Zero")),
	_values()
	{
		if (_columns > (std::numeric_limits<std::size_t>::max() / _rows)) {
			throw std::overflow_error("");
		}

		if (values.size() != (_rows * _columns)) {
			throw std::invalid_argument("Not Enough Or Too Many Values");
		}

		_values = values;
	}

	Values::Values(const TiXmlElement & element)
	{
		if (Values::XML_NAME != element.ValueStr()) {
			throw std::invalid_argument("Element Does Not Represent Class");
		}

		if (element.QueryValueAttribute<std::size_t>("Rows", &_rows)) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		if (element.QueryValueAttribute<std::size_t>("Columns", &_columns)) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		if (!_rows) {
			throw std::domain_error("Row Size Can't Be Zero");
		}

		if (!_columns) {
			throw std::domain_error("Column Size Can't Be Zero");
		}

		if (_columns > (std::numeric_limits<std::size_t>::max() / _rows)) {
			throw std::overflow_error("");
		}

		std::size_t index = 0;
		std::size_t size = (_rows * _columns);

		_values = std::valarray<double>(size);

		for (const TiXmlElement * child = element.FirstChildElement("Element"); child; child = child->NextSiblingElement("Element")) {
			if (child->QueryDoubleAttribute("Value", &_values[index])) {
				throw std::runtime_error("XML Error: Attribute Parsing");
			}

			index += 1;

			if (index == size) {
				throw std::runtime_error("XML Error: Element Parsing");
			}
		}

		if (index != size) {
			throw std::runtime_error("XML Error: Element Parsing");
		}
	}

	std::size_t Values::rows() const
	{
		return _rows;
	}

	std::size_t Values::columns() const
	{
		return _columns;
	}

	std::size_t Values::size() const
	{
		return _values.size();
	}

	std::valarray<double> Values::row(std::size_t index) const
	{
		if (index >= _rows) {
			throw std::out_of_range("Row Index Out Of Range");
		}

		return _values[std::slice((index * _columns), _columns, 1)];
	}

	std::valarray<double> Values::column(std::size_t index) const
	{
		if (index >= _columns) {
			throw std::out_of_range("Column Index Out Of Range");
		}

		return _values[std::slice(index, _rows, _columns)];
	}

	void Values::row(std::size_t index, const std::valarray<double> & values)
	{
		if (index >= _rows) {
			throw std::out_of_range("Row Index Out Of Range");
		}

		_values[std::slice((index * _columns), _columns, 1)] = values;
	}

	void Values::column(std::size_t index, const std::valarray<double> & values)
	{
		if (index >= _columns) {
			throw std::out_of_range("Column Index Out Of Range");
		}

		_values[std::slice(index, _rows, _columns)] = values;
	}

	double & Values::operator[](std::size_t index) {
		if (index >= _values.size()) {
			throw std::out_of_range("Index Out Of Range");
		}

		return _values[index];
	}

	const double & Values::operator[](std::size_t index) const {
		if (index >= _values.size()) {
			throw std::out_of_range("Index Out Of Range");
		}

		return _values[index];
	}

	double & Values::operator ()(std::size_t row, std::size_t column)
	{
		if (row >= _rows) {
			throw std::out_of_range("Row Index Out Of Range");
		}

		if (column >= _columns) {
			throw std::out_of_range("Column Index Out Of Range");
		}

		return _values[(row * _columns) + column];
	}

	const double & Values::operator ()(std::size_t row, std::size_t column) const
	{
		if (row >= _rows) {
			throw std::out_of_range("Row Index Out Of Range");
		}

		if (column >= _columns) {
			throw std::out_of_range("Column Index Out Of Range");
		}

		return _values[(row * _columns) + column];
	}

	Values & Values::operator +=(double rhs)
	{
		_values += rhs;
		return (* this);
	}

	Values & Values::operator -=(double rhs)
	{
		_values -= rhs;
		return (* this);
	}

	Values & Values::operator *=(double rhs)
	{
		_values *= rhs;
		return (* this);
	}

	Values & Values::operator /=(double rhs)
	{
		_values /= rhs;
		return (* this);
	}

	Values & Values::operator +=(const Values & rhs)
	{
		if (this == (& rhs)) {
			_values *= 2.0;
			return (* this);
		}

		if (_columns != rhs._columns) {
			throw std::invalid_argument("Different Columns #");
		}

		if (_rows != rhs._rows) {
			throw std::invalid_argument("Different Rows #");
		}

		_values += rhs._values;
		return (* this);
	}

	Values & Values::operator -=(const Values & rhs)
	{
		if (this == (& rhs)) {
			_values = 0.0;
			return (* this);
		}

		if (_columns != rhs._columns) {
			throw std::invalid_argument("Different Columns #");
		}

		if (_rows != rhs._rows) {
			throw std::invalid_argument("Different Rows #");
		}

		_values -= rhs._values;
		return (* this);
	}

	Values & Values::operator *=(const Values & rhs)
	{
		if (this == (& rhs)) {
			_values *= _values;
			return (* this);
		}

		if (_columns != rhs._columns) {
			throw std::invalid_argument("Different Columns #");
		}

		if (_rows != rhs._rows) {
			throw std::invalid_argument("Different Rows #");
		}

		_values *= rhs._values;
		return (* this);
	}

	Values & Values::operator /=(const Values & rhs)
	{
		if (this == (& rhs)) {
			_values /= _values;
			return (* this);
		}

		if (_columns != rhs._columns) {
			throw std::invalid_argument("Different Columns #");
		}

		if (_rows != rhs._rows) {
			throw std::invalid_argument("Different Rows #");
		}

		_values /= rhs._values;
		return (* this);
	}

	std::valarray<double> Values::_init_rows(const std::valarray<double> & values)
	{
		if (_columns > (std::numeric_limits<std::size_t>::max() / _rows)) {
			throw std::overflow_error("");
		}

		std::valarray<double> result(_rows * _columns);

		for (std::size_t index = 0; index < _rows; index++) {
			result[std::slice((index * _columns), _columns, 1)] = values[index];
		}

		return result;
	}

	std::valarray<double> Values::_init_columns(const std::valarray<double> & values)
	{
		if (_columns > (std::numeric_limits<std::size_t>::max() / _rows)) {
			throw std::overflow_error("");
		}

		std::valarray<double> result(_rows * _columns);

		for (std::size_t index = 0; index < _columns; index++) {
			result[std::slice(index, _rows, _columns)] = values[index];
		}

		return result;
	}

	std::ostream & operator <<(std::ostream & out, const Values & matrix)
	{
		std::valarray<double> values = matrix._values;
		std::size_t c_size = matrix.columns(), counter = 0, size = values.size();

		for (std::size_t index = 0; index < size; index++) {
			counter += 1;
			out << values[index] << " ";

			if (!(c_size - counter)) {
				counter = 0;
				out << std::endl;
			}
		}

		return out;
	}

	TiXmlElement Values::save_xml() const
	{
		std::size_t size = _values.size();
		TiXmlElement result(Values::XML_NAME);

		result.SetAttribute("Rows", boost::lexical_cast<std::string>(_rows));
		result.SetAttribute("Columns", boost::lexical_cast<std::string>(_columns));

		for (std::size_t index = 0; index < size; index++) {
			TiXmlElement value("Element");

			value.SetDoubleAttribute("Value", _values[index]);

			if (!result.InsertEndChild(value)) {
				throw std::runtime_error("XML Error: Element Assembling");
			}
		}

		return result;
	}

	Values operator +(double lhs, const Values & rhs)
	{
		Values result(rhs);

		result += lhs;
		return result;
	};

	Values operator -(double lhs, const Values & rhs)
	{
		Values result(rhs);

		result -= lhs;
		return result;
	};

	Values operator *(double lhs, const Values & rhs)
	{
		Values result(rhs);

		result *= lhs;
		return result;
	};

	Values operator /(double lhs, const Values & rhs)
	{
		Values result(rhs);

		result /= lhs;
		return result;
	};

	Values operator +(const Values & lhs, double rhs)
	{
		Values result(lhs);

		result += rhs;
		return result;
	};

	Values operator -(const Values & lhs, double rhs)
	{
		Values result(lhs);

		result -= rhs;
		return result;
	};

	Values operator *(const Values & lhs, double rhs)
	{
		Values result(lhs);

		result *= rhs;
		return result;
	};

	Values operator /(const Values & lhs, double rhs)
	{
		Values result(lhs);

		result /= rhs;
		return result;
	};

	Values operator +(const Values & lhs, const Values & rhs)
	{
		Values result(lhs);

		result += rhs;
		return result;
	};

	Values operator -(const Values & lhs, const Values & rhs)
	{
		Values result(lhs);

		result -= rhs;
		return result;
	};

	Values operator *(const Values & lhs, const Values & rhs)
	{
		Values result(lhs);

		result *= rhs;
		return result;
	};

	Values operator /(const Values & lhs, const Values & rhs)
	{
		Values result(lhs);

		result /= rhs;
		return result;
	};
}