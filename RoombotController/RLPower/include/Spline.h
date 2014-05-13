#ifndef SPLINE_H
#define	SPLINE_H

#include "Interval.h"

#include "tinyxmlplus.h"

// <editor-fold defaultstate="collapsed" desc="Library Includes">

#include <boost/utility.hpp>
#include <gsl/gsl_spline.h>

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="STL Includes">

#include <string>
#include <valarray>

// </editor-fold>

namespace POWER
{

	class Spline
	:
	public boost::noncopyable
	{
	  public:

		/**
		 * Class name for XML
		 */
		static const std::string XML_NAME;

		// <editor-fold defaultstate="collapsed" desc="Constructors">

		/*Spline(double, double, const std::valarray<double> &);*/
		Spline(const std::valarray<double> &, const std::valarray<double> &);
		/*Spline(const Interval &, const std::valarray<double> &);*/

		Spline(const TiXmlElement &);

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Destructor">

		virtual ~Spline();

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Accessors">

		/**
		 * Returns the interval over which the spline is defined
		 * @return Interval over which the spline is defined
		 */
		Interval x() const;

		/**
		 * Returns the interpolated value of a point contained in the interval
		 * @param - Point included in the interval over which the spline is
		 * defined
		 * @return Value of the point
		 */
		double y(double);

		/**
		 * Returns the interpolated values of a set of points contained in the interval
		 * @param - Set of point included in the interval over which the spline is
		 * defined
		 * @return Set of values one for each point
		 */
		std::valarray<double> y(const std::valarray<double> &);

		/**
		 * Returns the number of points used to define the spline
		 * @return # of points used to define the spline
		 */
		std::size_t size() const;

		/**
		 * Returns the spline type
		 * @return Type of the spline
		 */
		std::string type() const;

		// </editor-fold>

		/**
		 * Save class internal data in XML format
		 * @return XML Element representing class
		 */
		TiXmlElement save_xml() const;

	  private:

		static const gsl_interp_type * TYPE;

		// <editor-fold defaultstate="collapsed" desc="Members">

		gsl_spline * _spline;
		gsl_interp_accel * _accelerator;

		// </editor-fold>
	};
}

#endif	/* SPLINE_H */