#include "Spline.h"

#include <boost/lexical_cast.hpp>
#include <gsl/gsl_errno.h> // Implement Checks For Errors //

#include <stdexcept>

namespace POWER
{
	const std::string Spline::XML_NAME = "Spline";
	const gsl_interp_type * Spline::TYPE = gsl_interp_cspline_periodic;

	// <editor-fold defaultstate="collapsed" desc="Constructors">

	Spline::Spline(const std::valarray<double> & x, const std::valarray<double> & y)
	:
	_spline((x.size() == y.size()) ? gsl_spline_alloc(Spline::TYPE, x.size()) : throw std::invalid_argument("Incompatible Sizes")),
	_accelerator(gsl_interp_accel_alloc())
	{
		if (y[0] != y[y.size() - 1]) {
			throw std::invalid_argument("First Y Value Should Be Equal To Last");
		}

		if (!_spline) {
			throw std::bad_alloc();
		}

		if (!_accelerator) {
			throw std::bad_alloc();
		}

		if (0 != gsl_spline_init(_spline, &x[0], &y[0], _spline->size)) {
			throw std::runtime_error("Error Initializing Spline");
		}
	}

	Spline::Spline(const TiXmlElement & element)
	{
		if (Spline::XML_NAME != element.ValueStr()) {
			throw std::invalid_argument("Element Does Not Represent Class");
		}

		std::size_t size;

		if (element.QueryValueAttribute<std::size_t>("Size", &size)) {
			throw std::runtime_error("XML Error: Attribute Parsing");
		}

		const TiXmlElement * x_el = element.FirstChildElement("X");
		const TiXmlElement * y_el = element.FirstChildElement("Y");

		if ((!x_el) || (!y_el)) {
			throw std::runtime_error("XML Error: Element Parsing");
		}

		std::size_t index = 0;
		std::valarray<double> x(size);
		std::valarray<double> y(size);

		for (const TiXmlElement * child = x_el->FirstChildElement("Element"); child; child = child->NextSiblingElement("Element")) {
			if (child->QueryDoubleAttribute("Value", &x[index])) {
				throw std::runtime_error("XML Error: Attribute Parsing");
			}

			index += 1;
		}

		if (index != size) {
			throw std::runtime_error("XML Error: Element Parsing");
		}

		index = 0;

		for (const TiXmlElement * child = y_el->FirstChildElement("Element"); child; child = child->NextSiblingElement("Element")) {
			if (child->QueryDoubleAttribute("Value", &y[index])) {
				throw std::runtime_error("XML Error: Attribute Parsing");
			}

			index += 1;
		}

		if (index != size) {
			throw std::runtime_error("XML Error: Element Parsing");
		}

		if (y[0] != y[size - 1]) {
			throw std::invalid_argument("First Y Value Must Be Equal To Last");
		}

		_spline = gsl_spline_alloc(Spline::TYPE, size);

		if (!_spline) {
			throw std::bad_alloc();
		}

		_accelerator = gsl_interp_accel_alloc();

		if (!_accelerator) {
			throw std::bad_alloc();
		}

		if (0 != gsl_spline_init(_spline, &x[0], &y[0], _spline->size)) {
			throw std::runtime_error("Error Initializing Spline");
		}
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Destructor">

	Spline::~Spline()
	{
		if (_spline) {
			gsl_spline_free(_spline);
		}

		if (_accelerator) {
			gsl_interp_accel_free(_accelerator);
		}
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Accessors">

	Interval Spline::x() const
	{
		return Interval(_spline->interp->xmin, _spline->interp->xmax, _spline->size);
	}

	double Spline::y(double x)
	{
		return gsl_spline_eval(_spline, x, _accelerator);
	}

	std::valarray<double> Spline::y(const std::valarray<double> & x)
	{
		std::size_t size = x.size();
		std::valarray<double> result(size);

		for (std::size_t index = 0; index < size; index++) {

			result[index] = gsl_spline_eval(_spline, x[index], _accelerator);
		}

		return result;
	}

	std::size_t Spline::size() const
	{
		return _spline->size;
	}

	std::string Spline::type() const
	{
		return gsl_spline_name(_spline);
	}

	// </editor-fold>

	TiXmlElement Spline::save_xml() const
	{
		std::size_t size = _spline->size;
		TiXmlElement result(Spline::XML_NAME);
		TiXmlElement values_x("X");
		TiXmlElement values_y("Y");

		result.SetAttribute("Size", boost::lexical_cast<std::string>(size));

		for (std::size_t index = 0; index < size; index++) {
			TiXmlElement value("Element");

			value.SetDoubleAttribute("Value", _spline->x[index]);

			if (!values_x.InsertEndChild(value)) {
				throw std::runtime_error("XML Error: Element Assembling");
			}
		}

		for (std::size_t index = 0; index < size; index++) {
			TiXmlElement value("Element");

			value.SetDoubleAttribute("Value", _spline->y[index]);

			if (!values_y.InsertEndChild(value)) {
				throw std::runtime_error("XML Error: Element Assembling");
			}
		}

		if (!result.InsertEndChild(values_x)) {
			throw std::runtime_error("XML Error: Element Assembling");
		}

		if (!result.InsertEndChild(values_y)) {
			throw std::runtime_error("XML Error: Element Assembling");
		}

		return result;
	}
}