#include "Quaternion.h"

#include "cmath"

namespace transforms
{
	const std::string CLASS_NAME = "Quaternion";

	const double Quaternion::EPSILON(std::numeric_limits<double>::epsilon());
	const Quaternion Quaternion::ZERO(0,0,0,0);
	const Quaternion Quaternion::IDENTITY(1,0,0,0);
	const Quaternion Quaternion::ROTATION_X_90(0.70710678118654746,0.70710678118654746,0,0);
	const Quaternion Quaternion::ROTATION_Y_90(0.70710678118654746,0,0.70710678118654746,0);
	const Quaternion Quaternion::ROTATION_Z_90(0.70710678118654746,0,0,0.70710678118654746);
	const Quaternion Quaternion::ROTATION_Y_180(0,0,1,0);
	const Quaternion Quaternion::ROTATION_Y_270(-0.70710678118654746,0,0.70710678118654746,0);

	// <editor-fold defaultstate="collapsed" desc="Constructors">

	Quaternion::Quaternion()
	:
	_a(0),
	_b(0),
	_c(0),
	_d(0)
	{
	}

	Quaternion::Quaternion(const double & a, const double & b, const double & c, const double & d)
	:
	_a(a),
	_b(b),
	_c(c),
	_d(d)
	{
	}

	Quaternion::Quaternion(const Vector_4 & rotation)
	{
		(* this) = rotation;
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Getters">

	double Quaternion::a() const
	{
		return _a;
	}

	double Quaternion::b() const
	{
		return _b;
	}

	double Quaternion::c() const
	{
		return _c;
	}

	double Quaternion::d() const
	{
		return _d;
	}

	// </editor-fold>

	Quaternion Quaternion::operator +() const
	{
		return Quaternion(_a, _b, _c, _d);
	}

	Quaternion Quaternion::operator -() const
	{
		return Quaternion(-_a, -_b, -_c, -_d);
	}

	// <editor-fold defaultstate="collapsed" desc="Assignment Operators">

	// <editor-fold defaultstate="collapsed" desc="Simple Assignment">

	Quaternion & Quaternion::operator =(const double & rhs)
	{
		_a = rhs;
		_b = _c = _d = 0.0;

		return (* this);
	}

	Quaternion & Quaternion::operator =(const Vector_4 & rhs)
	{
		double half_angle, sin_ha, cos_ha;
		double x = rhs.x(), y = rhs.y(), z = rhs.z();

		half_angle = (rhs.a() * 0.5);
		sin_ha = std::sin(half_angle);
		cos_ha = std::cos(half_angle);

		if(Quaternion::EPSILON > std::abs(cos_ha)) {
			cos_ha = 0;
		}

		if(Quaternion::EPSILON > std::abs(sin_ha)) {
			sin_ha = 0;
		}

		_a = cos_ha;
		_b = x * sin_ha;
		_c = y * sin_ha;
		_d = z * sin_ha;

		return (* this);
	}

	/*Quaternion & Quaternion::operator =(const Quaternion & rhs)
	{
		if(this != (& rhs)) {
			_a = rhs._a;
			_b = rhs._b;
			_c = rhs._c;
			_d = rhs._d;
		}

		return (* this);
	}*/

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Compound Assignment">

	Quaternion & Quaternion::operator +=(const double & rhs)
	{
		_a += rhs;

		return (* this);
	}

	Quaternion & Quaternion::operator -=(const double & rhs)
	{
		_a -= rhs;

		return (* this);
	}

	Quaternion & Quaternion::operator *=(const double & rhs)
	{
		_a *= rhs;
		_b *= rhs;
		_c *= rhs;
		_d *= rhs;

		return (* this);
	}

	Quaternion & Quaternion::operator /=(const double & rhs)
	{
		_a /= rhs;
		_b /= rhs;
		_c /= rhs;
		_d /= rhs;

		return (* this);
	}

	Quaternion & Quaternion::operator +=(const Quaternion & rhs)
	{
		_a += rhs.a();
		_b += rhs.b();
		_c += rhs.c();
		_d += rhs.d();

		return (* this);
	}

	Quaternion & Quaternion::operator -=(const Quaternion & rhs)
	{
		_a -= rhs.a();
		_b -= rhs.b();
		_c -= rhs.c();
		_d -= rhs.d();

		return (* this);
	}

	Quaternion & Quaternion::operator *=(const Quaternion & rhs)
	{
		double ar = rhs.a();
		double br = rhs.b();
		double cr = rhs.c();
		double dr = rhs.d();

		double at = +_a*ar - _b*br - _c * cr - _d*dr;
		double bt = +_a*br + _b*ar + _c * dr - _d*cr; //(a*br+ar*b)+(c*dr-cr*d);
		double ct = +_a*cr - _b*dr + _c * ar + _d*br; //(a*cr+ar*c)+(d*br-dr*b);
		double dt = +_a*dr + _b*cr - _c * br + _d*ar; //(a*dr+ar*d)+(b*cr-br*c);

		_a = at;
		_b = bt;
		_c = ct;
		_d = dt;

		return (* this);
	}

	Quaternion & Quaternion::operator /=(const Quaternion & rhs)
	{
		double ar = rhs.a();
		double br = rhs.b();
		double cr = rhs.c();
		double dr = rhs.d();

		double denominator = ar * ar + br * br + cr * cr + dr*dr;

		double at = (+_a * ar + _b * br + _c * cr + _d * dr) / denominator; //(a*ar+b*br+c*cr+d*dr)/denominator;
		double bt = (-_a * br + _b * ar - _c * dr + _d * cr) / denominator; //((ar*b-a*br)+(cr*d-c*dr))/denominator;
		double ct = (-_a * cr + _b * dr + _c * ar - _d * br) / denominator; //((ar*c-a*cr)+(dr*b-d*br))/denominator;
		double dt = (-_a * dr - _b * cr + _c * br + _d * ar) / denominator; //((ar*d-a*dr)+(br*c-b*cr))/denominator;

		_a = at;
		_b = bt;
		_c = ct;
		_d = dt;

		return (* this);
	}

	// </editor-fold>

	std::ostream & operator << (std::ostream & out, const Quaternion & quaternion)
	{
            out << "("
				<< quaternion._a
				<< ", "
				<< quaternion._b
				<< ", "
				<< quaternion._c
				<< ", "
				<< quaternion._d
				<< ")";
            return out;
	}

	// </editor-fold>

	const double * Quaternion::c_array() const
	{
		return (& _a);
	}

	void Quaternion::normalise()
	{
		double lenght = ((_a * _a) + (_b * _b) + (_c * _c) + (_d * _d));

		if(0.0 == lenght) {
			return;
		}

        double inverse_lenght = 1.0 / std::sqrt(lenght);

		_a *= inverse_lenght;
		_b *= inverse_lenght;
		_c *= inverse_lenght;
		_d *= inverse_lenght;
	}

	Quaternion Quaternion::conjugate() const
	{
		return Quaternion(_a, -_b, -_c, -_d);
	}

	Vector_4 Quaternion::to_axis_angle() const
	{
		double square_lenght = ((_b * _b) + (_c * _c) + (_d * _d));
		double inverse_lenght, angle, x, y, z;

        if(0.0 < square_lenght) {
			inverse_lenght = 1.0 / std::sqrt(square_lenght);
			x = _b * inverse_lenght;
			y = _c * inverse_lenght;
			z = _d * inverse_lenght;
			//angle = 2.0 * acos(_a);

			if(-1.0 < angle) {
				if(1.0 > angle) {
					angle = 2.0 * std::acos(_a);
				}

				else {
					angle = 0.0;
				}
			}

			else {
				angle = 2.0 * (4.0 * std::atan (1.0));
			}
        }

        else {
			x = 0.0;
			y = 1.0;
			z = 0.0;
			angle = 0.0;
        }

		return Vector_4(x, y, z, angle);
	}

	// <editor-fold defaultstate="collapsed" desc="Interfaces">

	boost::property_tree::ptree Quaternion::export_to_ptree() const
	{
		boost::property_tree::ptree result;

		result.add<double>("A", _a);
		result.add<double>("B", _b);
		result.add<double>("C", _c);
		result.add<double>("D", _d);

		return result;
	}

	void Quaternion::import_from_ptree(const boost::property_tree::ptree & root)
	{
		_a = root.get<double>("A");
		_b = root.get<double>("B");
		_c = root.get<double>("C");
		_d = root.get<double>("D");
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Arithmetic Operators">

	Quaternion operator +(const double & lhs, const Quaternion & rhs)
	{
		Quaternion result(lhs);
		result += rhs;
		return result;
	}

	Quaternion operator -(const double & lhs, const Quaternion & rhs)
	{
		Quaternion result(lhs);
		result -= rhs;
		return result;
	}

	Quaternion operator *(const double & lhs, const Quaternion & rhs)
	{
		Quaternion result(lhs);
		result *= rhs;
		return result;
	}

	Quaternion operator /(const double & lhs, const Quaternion & rhs)
	{
		Quaternion result(lhs);
		result /= rhs;
		return result;
	}

	Quaternion operator +(const Quaternion & lhs, const double & rhs)
	{
		Quaternion result(lhs);
		result += rhs;
		return result;
	}

	Quaternion operator -(const Quaternion & lhs, const double & rhs)
	{
		Quaternion result(lhs);
		result -= rhs;
		return result;
	}

	Quaternion operator *(const Quaternion & lhs, const double & rhs)
	{
		Quaternion result(lhs);
		result *= rhs;
		return result;
	}

	Quaternion operator /(const Quaternion & lhs, const double & rhs)
	{
		Quaternion result(lhs);
		result /= rhs;
		return result;
	}

	Quaternion operator +(const Quaternion & lhs, const Quaternion & rhs)
	{
		Quaternion result(lhs);
		result += rhs;
		return result;
	}

	Quaternion operator -(const Quaternion & lhs, const Quaternion & rhs)
	{
		Quaternion result(lhs);
		result -= rhs;
		return result;
	}

	Quaternion operator *(const Quaternion & lhs, const Quaternion & rhs)
	{
		Quaternion result(lhs);
		result *= rhs;
		return result;
	}

	Quaternion operator /(const Quaternion & lhs, const Quaternion & rhs)
	{
		Quaternion result(lhs);
		result /= rhs;
		return result;
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Boolean Operators">

	bool operator ==(const double & lhs, const Quaternion & rhs)
	{
		return ((rhs.a() == lhs) &&
				(rhs.b() == 0.0) &&
				(rhs.c() == 0.0) &&
				(rhs.d() == 0.0));
	}

	bool operator ==(const Quaternion & lhs, const double & rhs)
	{
		return ((lhs.a() == rhs) &&
				(lhs.b() == 0.0) &&
				(lhs.c() == 0.0) &&
				(lhs.d() == 0.0));
	}

	bool operator ==(const Quaternion & lhs, const Quaternion & rhs)
	{
		return ((lhs.a() == rhs.a()) &&
				(lhs.b() == rhs.b()) &&
				(lhs.c() == rhs.c()) &&
				(lhs.d() == rhs.d()));
	}

	bool operator !=(const double & lhs, const Quaternion & rhs)
	{
		return (! (lhs == rhs));
	}

	bool operator !=(const Quaternion & lhs, const double & rhs)
	{
		return (! (lhs == rhs));
	}

	bool operator !=(const Quaternion & lhs, const Quaternion & rhs)
	{
		return (! (lhs == rhs));
	}

	// </editor-fold>
}