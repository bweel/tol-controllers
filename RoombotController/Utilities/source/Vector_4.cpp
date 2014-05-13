#include "Vector_4.h"

namespace transforms
{
	const std::string CLASS_NAME = "Vector_4";
	const Vector_4 Vector_4::ZERO(0,0,0,0);

	// <editor-fold defaultstate="collapsed" desc="Constructors">

	Vector_4::Vector_4()
	:
	_x(0),
	_y(0),
	_z(0),
	_a(0)
	{
	}

	Vector_4::Vector_4(const double & scalar)
	:
	_x(scalar),
	_y(scalar),
	_z(scalar),
	_a(scalar)
	{
	}

	Vector_4::Vector_4(const double & x, const double & y, const double & z, const double & a)
	:
	_x(x),
	_y(y),
	_z(z),
	_a(a)
	{
	}

	Vector_4::Vector_4(const double * array)
	:
	_x(array[0]),
	_y(array[1]),
	_z(array[2]),
	_a(array[3])
	{
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Getters">

	double Vector_4::x() const
	{
		return _x;
	}

	double Vector_4::y() const
	{
		return _y;
	}

	double Vector_4::z() const
	{
		return _z;
	}

	double Vector_4::a() const
	{
		return _a;
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Unary Operators">

	Vector_4 Vector_4::operator +() const
	{
		return Vector_4(_x, _y, _z, _a);
	}

	Vector_4 Vector_4::operator -() const
	{
		return Vector_4(-_x, -_y, -_z, -_a);
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Binary Operators">

	// <editor-fold defaultstate="collapsed" desc="Simple Assignment">

	Vector_4 & Vector_4::operator =(const double & rhs)
	{
		_x = rhs;
		_y = rhs;
		_z = rhs;
		_a = rhs;

		return (* this);
	}
	
	Vector_4 & Vector_4::operator =(const double * rhs)
	{
		_x = rhs[0];
		_y = rhs[1];
		_z = rhs[2];
		_a = rhs[3];

		return (* this);
	}

	Vector_4 & Vector_4::operator =(const Vector_4 & rhs)
	{
		if(this != (& rhs))	{
			_x = rhs._x;
			_y = rhs._y;
			_z = rhs._z;
			_a = rhs._a;
		}

		return (* this);
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Compound Assignment">

	Vector_4 & Vector_4::operator +=(const double & rhs)
	{
		_x += rhs;
		_y += rhs;
		_z += rhs;
		_a += rhs;

		return (* this);
	}

	Vector_4 & Vector_4::operator -=(const double & rhs)
	{
		_x -= rhs;
		_y -= rhs;
		_z -= rhs;
		_a -= rhs;

		return (* this);
	}

	Vector_4 & Vector_4::operator *=(const double & rhs)
	{
		_x *= rhs;
		_y *= rhs;
		_z *= rhs;
		_a *= rhs;

		return (* this);
	}

	Vector_4 & Vector_4::operator /=(const double & rhs)
	{
		if (rhs != 0.0)
		{
			double inverse = 1.0 / rhs;

			_x *= inverse;
			_y *= inverse;
			_z *= inverse;
			_a *= inverse;
		}

		return (* this);
	}

	Vector_4 & Vector_4::operator +=(const Vector_4 & rhs)
	{
		_x += rhs.x();
		_y += rhs.y();
		_z += rhs.z();
		_a += rhs.a();

		return (* this);
	}

	Vector_4 & Vector_4::operator -=(const Vector_4 & rhs)
	{
		_x -= rhs.x();
		_y -= rhs.y();
		_z -= rhs.z();
		_a -= rhs.a();

		return (* this);
	}

	Vector_4 & Vector_4::operator *=(const Vector_4 & rhs)
	{
		_x *= rhs.x();
		_y *= rhs.y();
		_z *= rhs.z();
		_a *= rhs.a();

		return (* this);
	}

	Vector_4 & Vector_4::operator /=(const Vector_4 & rhs)
	{
		_x /= rhs.x();
		_y /= rhs.y();
		_z /= rhs.z();
		_a /= rhs.a();

		return (* this);
	}

	// </editor-fold>

	std::ostream & operator << (std::ostream & out, const Vector_4 & vector)
	{
		out << "("
			<< vector._x
			<< ", "
			<< vector._y
			<< ", "
			<< vector._z
			<< ", "
			<< vector._a
			<< ")";
		return out;
	}

	// </editor-fold>

	const double * Vector_4::c_array() const
	{
		return (& _x);
	}

	// <editor-fold defaultstate="collapsed" desc="Interfaces">

	boost::property_tree::ptree Vector_4::export_to_ptree() const
	{
		boost::property_tree::ptree result;

		result.add("X", _x);
		result.add("Y", _y);
		result.add("Z", _z);
		result.add("A", _a);

		return result;
	}

	void Vector_4::import_from_ptree(const boost::property_tree::ptree & root)
	{
		_x = root.get<double>("X");
		_y = root.get<double>("Y");
		_z = root.get<double>("Z");
		_a = root.get<double>("A");
	}

	// </editor-fold>

	Vector_4 operator + (const double & lhs, const Vector_4 & rhs)
	{
		Vector_4 result(lhs);
		result += rhs;
		return result;
	}

	Vector_4 operator - (const double & lhs, const Vector_4 & rhs)
	{
		Vector_4 result(lhs);
		result -= rhs;
		return result;
	}

	Vector_4 operator * (const double & lhs, const Vector_4 & rhs)
	{
		Vector_4 result(lhs);
		result *= rhs;
		return result;
	}

	Vector_4 operator / (const double & lhs, const Vector_4 & rhs)
	{
		Vector_4 result(lhs);
		result /= rhs;
		return result;
	}

	Vector_4 operator + (const Vector_4 & lhs, const double & rhs)
	{
		Vector_4 result(lhs);
		result += rhs;
		return result;
	}

	Vector_4 operator - (const Vector_4 & lhs, const double & rhs)
	{
		Vector_4 result(lhs);
		result -= rhs;
		return result;
	}

	Vector_4 operator * (const Vector_4 & lhs, const double & rhs)
	{
		Vector_4 result(lhs);
		result *= rhs;
		return result;
	}

	Vector_4 operator / (const Vector_4 & lhs, const double & rhs)
	{
		Vector_4 result(lhs);
		result /= rhs;
		return result;
	}

	Vector_4 operator + (const Vector_4 & lhs, const Vector_4 & rhs)
	{
		Vector_4 result(lhs);
		result += rhs;
		return result;
	}

	Vector_4 operator - (const Vector_4 & lhs, const Vector_4 & rhs)
	{
		Vector_4 result(lhs);
		result -= rhs;
		return result;
	}

	Vector_4 operator * (const Vector_4 & lhs, const Vector_4 & rhs)
	{
		Vector_4 result(lhs);
		result *= rhs;
		return result;
	}

	Vector_4 operator / (const Vector_4 & lhs, const Vector_4 & rhs)
	{
		Vector_4 result(lhs);
		result /= rhs;
		return result;
	}

	bool operator ==(const Vector_4 & lhs, const Vector_4 & rhs)
	{
		return ((lhs.x() == rhs.x()) &&
				(lhs.y() == rhs.y()) &&
				(lhs.z() == rhs.z()) &&
				(lhs.a() == rhs.a()));
	}

	bool operator !=(const Vector_4 & lhs, const Vector_4 & rhs)
	{
		return(! (lhs == rhs));
	}
}