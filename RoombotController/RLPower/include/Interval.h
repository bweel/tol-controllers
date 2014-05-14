#ifndef INTERVAL_H
#define	INTERVAL_H

#include <tinyxmlplus.h>

#include <cstddef>
#include <valarray>

namespace POWER
{

	class Interval
	{
	  public:

		/**
		 * Minimum number of points in an interval
		 */
		static const std::size_t I_SIZE_MIN;

		/**
		 * Maximum number of points in an interval
		 */
		static const std::size_t I_SIZE_MAX;

		/**
		 * Class name for XML
		 */
		static const std::string XML_NAME;

		// <editor-fold defaultstate="collapsed" desc="Constructor">

		/**
		 * Creates an interval [Start,End] containing n points defined at regular
		 * subintervals
		 * @param Start point on real axis
		 * @param End point on real axis
		 * @param Number of points including start and end points
		 */
		Interval(double, double, std::size_t = Interval::I_SIZE_MIN);

		/**
		 * Creates an interval from an XML object representation
		 * @param XML element representing object
		 */
		Interval(const TiXmlElement &);

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Accessors">

		/**
		 * Returns the point with the minimum value in the interval
		 * @return Minimum
		 */
		inline double min() const
		{
			return _start;
		}

		/**
		 * Returns the point with the maximum value in the interval
		 * @return Maximum
		 */
		inline double max() const
		{
			return _end;
		}

		/**
		 * Returns the length of the interval
		 * @return Length of the interval
		 */
		inline double lenght() const
		{
			return _length;
		}

		/**
		 * Returns the total number of points in the interval, extremes included
		 * @return Total number of points in the interval
		 */
		inline std::size_t size() const
		{
			return _x.size();
		}

		/**
		 * Returns the values of all the points in the interval
		 * @return Points values
		 */
		inline std::valarray<double> x() const
		{
			return _x;
		}

		/**
		 * Returns the values of a desired number of points in the interval
		 * @param Number of points in the interval
		 * @return Points values
		 */
		std::valarray<double> x(std::size_t) const;

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Mutator">

		/**
		 * Changes the number of points in the interval
		 * @param New number of points in interval
		 */
		void resize(std::size_t);

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Operator">

		/**
		 * Returns the value of the n-th point in the interval
		 * @param Index of the n-th point in the interval
		 * @return Point value
		 */
		const double & operator[](std::size_t) const;

		// </editor-fold>

		/**
		 * Save class internal data in XML format
		 * @return XML Element representing class
		 */
		TiXmlElement save_xml() const;

	  private:

		// <editor-fold defaultstate="collapsed" desc="Members">

		double _start;
		double _end;

		double _length;
		double _delta;

		std::valarray<double> _x;

		// </editor-fold>

	};
}

#endif	/* INTERVAL_H */