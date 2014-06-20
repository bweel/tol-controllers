#ifndef VALUES_H
#define	VALUES_H

#include <tinyxmlplus.h>

#include <cstddef>
#include <iostream>
#include <valarray>

namespace POWER
{

	class Values
	{
	  public:

		/**
		 * Class name for XML
		 */
		static const std::string XML_NAME;

		// <editor-fold defaultstate="collapsed" desc="Constructors">

		/**
		 * Creates a matrix of values with n rows and m columns initialized to 0
		 * or to a specified value.
		 *
		 * Example: n = 2, m = 2.
		 * (a,a;
		 *  a,a)
		 * @param Number of rows
		 * @param Number of columns
		 * @param Default value
		 */
		Values(std::size_t = 1, std::size_t = 1, double = 0.0);

		/**
		 * Creates a matrix of values with n rows and values.size() columns
		 *
		 * Example: n = 2, m = 2.
		 * (a,b;
		 *  a,b)
		 * @param Number of rows
		 * @param Init value array for each column
		 */
		Values(std::size_t, const std::valarray<double> &);

		/**
		 * Creates a matrix of values with values.size() rows and m columns
		 *
		 * Example: n = 2, m = 2.
		 * (a,a;
		 *  b,b)
		 * @param Init value array for each row
		 * @param Number of columns
		 */
		Values(const std::valarray<double> &, std::size_t);

		/**
		 * Create a matrix of values with n rows and m columns where each element
		 * is initialized by some value
		 *
		 * Example: n = 2, m = 2.
		 * (a,b;
		 *  c,d)
		 * @param Number of rows
		 * @param Number of columns
		 * @param Init value array for each element of the matrix
		 */
		Values(std::size_t, std::size_t, const std::valarray<double> &);

		Values(const TiXmlElement &);

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Accessors">

		/**
		 * Returns the matrix number of rows
		 * @return Number of rows
		 */
		std::size_t rows() const;

		/**
		 * Returns the matrix number of columns
		 * @return Number of columns
		 */
		std::size_t columns() const;

		/**
		 * Returns the matrix number of elements
		 * @return Number of elements
		 */
		std::size_t size() const;

		/**
		 * Returns the contents of the i-th row
		 * @param Row index
		 * @return Row contents
		 */
		std::valarray<double> row(std::size_t) const;

		/**
		 * Returns the contents of the j-th column
		 * @param Column index
		 * @return Column contents
		 */
		std::valarray<double> column(std::size_t) const;

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Mutators">

		void row(std::size_t, const std::valarray<double> &);
		void column(std::size_t, const std::valarray<double> &);

		// </editor-fold>

		// <editor-fold defaultstate="collapsed" desc="Operators">

		/**
		 * Returns a reference to the k-th mtrix element
		 * @param Element index
		 * @return Matrix element reference
		 */
		double & operator[](std::size_t);

		/**
		 * Returns a constant reference to the k-th mtrix element
		 * @param Element index
		 * @return Constant matrix element reference
		 */
		const double & operator[](std::size_t) const;

		/**
		 * Returns a reference to the matix element in i-th row and j-th column
		 * @param Row index
		 * @param Column index
		 * @return Matrix element reference
		 */
		double & operator()(std::size_t, std::size_t);

		/**
		 * Returns a constant reference to the matix element in i-th row and j-th column
		 * @param Row index
		 * @param Column index
		 * @return Constant matrix element reference
		 */
		const double & operator()(std::size_t, std::size_t) const;

		inline Values operator +() const
		{
			return Values(_rows, _columns, _values);
		}

		inline Values operator -() const
		{
			return Values(_rows, _columns, -_values);
		}

		Values & operator +=(double);
		Values & operator -=(double);
		Values & operator *=(double);
		Values & operator /=(double);

		Values & operator +=(const Values &);
		Values & operator -=(const Values &);
		Values & operator *=(const Values &);
		Values & operator /=(const Values &);

		friend std::ostream & operator <<(std::ostream &, const Values &);
		//friend std::istream & operator >> (std::istream &, Values &);

		// </editor-fold>

		/**
		 * Save class internal data in XML format
		 * @return XML Element representing class
		 */
		TiXmlElement save_xml() const;

	  private:

		std::size_t _rows;
		std::size_t _columns;

		std::valarray<double> _values;

		std::valarray<double> _init_rows(const std::valarray<double> &);
		std::valarray<double> _init_columns(const std::valarray<double> &);
	};

	Values operator+(double, const Values &);
	Values operator-(double, const Values &);
	Values operator*(double, const Values &);
	Values operator/(double, const Values &);
	Values operator+(const Values &, double);
	Values operator-(const Values &, double);
	Values operator*(const Values &, double);
	Values operator/(const Values &, double);
	Values operator+(const Values &, const Values &);
	Values operator-(const Values &, const Values &);
	Values operator*(const Values &, const Values &);
	Values operator/(const Values &, const Values &);
}
#endif	/* VALUES_H */