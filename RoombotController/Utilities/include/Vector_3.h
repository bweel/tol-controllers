#ifndef VECTOR_3_H
#define	VECTOR_3_H

//#include "Vector_4.h"
//#include "Quaternion.h"

#include "IExportable.h"

namespace transforms
{
	class Vector_3
	:
	virtual public IExportable
	{
		public:

			static const std::string CLASS_NAME;
			static const double EPSILON;
			static const Vector_3 ZERO;
			static const Vector_3 UNIT_X;
			static const Vector_3 UNIT_Y;
			static const Vector_3 UNIT_Z;

			// <editor-fold defaultstate="collapsed" desc="Constructors">

			Vector_3();
			Vector_3(const double &);
			Vector_3(const double &, const double &, const double &);
			//Vector_3(const double & coordinates[3]);
			Vector_3(const double *);

			// </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Getters">

			double x() const;
			double y() const;
			double z() const;

			// </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Unary Operators">

			Vector_3 operator +() const;
			Vector_3 operator -() const;

			// </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Binary Operators">

			// <editor-fold defaultstate="collapsed" desc="Simple Assigment">

			Vector_3 & operator = (const double &);
			Vector_3 & operator = (const double *);
			Vector_3 & operator = (const Vector_3 &);

			// </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Compound Assignment">

			Vector_3 & operator +=(const double &);
			Vector_3 & operator -=(const double &);
			Vector_3 & operator *=(const double &);
			Vector_3 & operator /=(const double &);
			Vector_3 & operator +=(const Vector_3 &);
			Vector_3 & operator -=(const Vector_3 &);
			Vector_3 & operator *=(const Vector_3 &);
			Vector_3 & operator /=(const Vector_3 &);
//			Vector_3 & operator *=(const Quaternion &);

			// </editor-fold>

			friend std::ostream & operator << (std::ostream &, const Vector_3 &);

			// </editor-fold>

			double lenght() const;
			double lenght_square() const;
			bool is_zero_lenght() const;
			void normalise();

			double dot_product(const Vector_3 &) const;
			Vector_3 cross_product(const Vector_3 &) const;
//			Quaternion calculate_rotation(const Vector_3 &) const;

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

			// </editor-fold>
			
	};

	/* Non Member Operator Overloads */

	Vector_3 operator +(const double &, const Vector_3 &);
	Vector_3 operator -(const double &, const Vector_3 &);
	Vector_3 operator *(const double &, const Vector_3 &);
	Vector_3 operator /(const double &, const Vector_3 &);

	Vector_3 operator +(const Vector_3 &, const double &);
	Vector_3 operator -(const Vector_3 &, const double &);
	Vector_3 operator *(const Vector_3 &, const double &);
	Vector_3 operator /(const Vector_3 &, const double &);

	Vector_3 operator +(const Vector_3 &, const Vector_3 &);
	Vector_3 operator -(const Vector_3 &, const Vector_3 &);
	Vector_3 operator *(const Vector_3 &, const Vector_3 &);
	Vector_3 operator /(const Vector_3 &, const Vector_3 &);

//	Vector_3 operator *(const Quaternion &, const Vector_3 &);
//	Vector_3 operator *(const Vector_3 &, const Quaternion &);

	bool operator ==(const Vector_3 &, const Vector_3 &);
	bool operator !=(const Vector_3 &, const Vector_3 &);

	bool operator <(const Vector_3 &, const Vector_3 &);
	bool operator >(const Vector_3 &, const Vector_3 &);
	//bool operator <=(const Vector_3 &, const Vector_3 &);
	//bool operator >=(const Vector_3 &, const Vector_3 &);

}
#endif	/* VECTOR_3_H */