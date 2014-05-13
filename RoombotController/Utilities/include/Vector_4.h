#ifndef VECTOR_4_H
#define	VECTOR_4_H

#include "IExportable.h"

namespace transforms
{
	class Vector_4
	:
	virtual public IExportable
	{
		public:

			static const std::string CLASS_NAME;
			static const Vector_4 ZERO;

			// <editor-fold defaultstate="collapsed" desc="Constructors">

			Vector_4();
			Vector_4(const double &);
			Vector_4(const double &, const double &, const double &, const double &);
			Vector_4(const double *);

			// </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Getters">

			double x() const;
			double y() const;
			double z() const;
			double a() const;

			// </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Unary Operators">

			Vector_4 operator +() const;
			Vector_4 operator -() const;

			// </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Binary Operators">

			// <editor-fold defaultstate="collapsed" desc="Simple Assignment">

			Vector_4 & operator =(const double &);
			Vector_4 & operator =(const double *);
			Vector_4 & operator =(const Vector_4 &);

			// </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Compound Assignment">

			Vector_4 & operator +=(const double &);
			Vector_4 & operator -=(const double &);
			Vector_4 & operator *=(const double &);
			Vector_4 & operator /=(const double &);
			Vector_4 & operator +=(const Vector_4 &);
			Vector_4 & operator -=(const Vector_4 &);
			Vector_4 & operator *=(const Vector_4 &);
			Vector_4 & operator /=(const Vector_4 &);

			// </editor-fold>

			friend std::ostream & operator << (std::ostream &, const Vector_4 &);

			// </editor-fold>

			const double * c_array() const;

			// <editor-fold defaultstate="collapsed" desc="Interfaces">

			boost::property_tree::ptree export_to_ptree() const;
			void import_from_ptree(const boost::property_tree::ptree &);

			// </editor-fold>

		private:

			// <editor-fold defaultstate="collapsed" desc="Members">

			double _x;
			double _y;
			double _z;
			double _a;

			// </editor-fold>
	};

	/* Non Member Operator Overloads */

	Vector_4 operator + (const double &, const Vector_4 &);
	Vector_4 operator - (const double &, const Vector_4 &);
	Vector_4 operator * (const double &, const Vector_4 &);
	Vector_4 operator / (const double &, const Vector_4 &);

	Vector_4 operator + (const Vector_4 &, const double &);
	Vector_4 operator - (const Vector_4 &, const double &);
	Vector_4 operator * (const Vector_4 &, const double &);
	Vector_4 operator / (const Vector_4 &, const double &);

	Vector_4 operator + (const Vector_4 &, const Vector_4 &);
	Vector_4 operator - (const Vector_4 &, const Vector_4 &);
	Vector_4 operator * (const Vector_4 &, const Vector_4 &);
	Vector_4 operator / (const Vector_4 &, const Vector_4 &);

	bool operator ==(const Vector_4 &, const Vector_4 &);
	bool operator !=(const Vector_4 &, const Vector_4 &);
}
#endif	/* VECTOR_4_H */