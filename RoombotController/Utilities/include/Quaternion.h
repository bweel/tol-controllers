#ifndef QUATERNION_H
#define	QUATERNION_H

#include "Vector_4.h"

#include "IExportable.h"

namespace transforms
{
	class Quaternion
	:
	virtual public IExportable
	{
		public:
			static const std::string CLASS_NAME;
			static const double EPSILON;
			static const Quaternion ZERO;
			static const Quaternion IDENTITY;
			static const Quaternion ROTATION_X_90;
			static const Quaternion ROTATION_Y_90;
			static const Quaternion ROTATION_Z_90;
			static const Quaternion ROTATION_Y_180;
			static const Quaternion ROTATION_Y_270;

			Quaternion();
			Quaternion(const double &, const double &, const double &, const double &);
			Quaternion(const Vector_4 &);

			// <editor-fold defaultstate="collapsed" desc="Getters">

			double b() const;
			double c() const;
			double d() const;
			double a() const;

			// </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Unary Operators">

			Quaternion operator +() const;
			Quaternion operator -() const;

			// </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Binary Operators">

			// <editor-fold defaultstate="collapsed" desc="Simple Assigment">

			Quaternion & operator = (const double &);
			Quaternion & operator = (const Vector_4 &);
			//Quaternion & operator = (const Quaternion &);

			// </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Compound Assignment">

			//Quaternion & operator *=(const Vector_3 &);
			Quaternion & operator +=(const double &);
			Quaternion & operator -=(const double &);
			Quaternion & operator *=(const double &);
			Quaternion & operator /=(const double &);
			Quaternion & operator +=(const Quaternion &);
			Quaternion & operator -=(const Quaternion &);
			Quaternion & operator *=(const Quaternion &);
			Quaternion & operator /=(const Quaternion &);

			// </editor-fold>

			// </editor-fold>

			friend std::ostream & operator << (std::ostream &, const Quaternion &);

			void normalise();
			Quaternion conjugate() const;
			Vector_4 to_axis_angle() const;

			const double * c_array() const;

			// <editor-fold defaultstate="collapsed" desc="Interfaces">

			boost::property_tree::ptree export_to_ptree() const;
			void import_from_ptree(const boost::property_tree::ptree &);

			// </editor-fold>

		private:

			// <editor-fold defaultstate="collapsed" desc="Members">

			double _a;
			double _b;
			double _c;
			double _d;

			// </editor-fold>

	};

	/* Non Member Operator Overloads */
	Quaternion operator + (const double &, const Quaternion &);
	Quaternion operator - (const double &, const Quaternion &);
	Quaternion operator * (const double &, const Quaternion &);
	Quaternion operator / (const double &, const Quaternion &);

	Quaternion operator + (const Quaternion &, const double &);
	Quaternion operator - (const Quaternion &, const double &);
	Quaternion operator * (const Quaternion &, const double &);
	Quaternion operator / (const Quaternion &, const double &);

	Quaternion operator + (const Quaternion &, const Quaternion &);
	Quaternion operator - (const Quaternion &, const Quaternion &);
	Quaternion operator * (const Quaternion &, const Quaternion &);
	Quaternion operator / (const Quaternion &, const Quaternion &);

	bool operator == (const double &, const Quaternion &);
	bool operator == (const Quaternion &, const double &);
	bool operator == (const Quaternion &, const Quaternion &);

	bool operator != (const double &, const Quaternion &);
	bool operator != (const Quaternion &, const double &);
	bool operator != (const Quaternion &, const Quaternion &);
}

#endif	/* QUATERNION_H */