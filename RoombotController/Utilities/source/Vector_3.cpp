#include "Vector_3.h"

#include <cmath>

namespace transforms
{
	const std::string CLASS_NAME = "Vector_3";
	const double Vector_3::EPSILON(std::numeric_limits<double>::epsilon());
	const Vector_3 Vector_3::ZERO(0, 0, 0);
	const Vector_3 Vector_3::UNIT_X(1, 0, 0);
	const Vector_3 Vector_3::UNIT_Y(0, 1, 0);
	const Vector_3 Vector_3::UNIT_Z(0, 0, 1);

	// <editor-fold defaultstate="collapsed" desc="Constructors">

	Vector_3::Vector_3()
	:
	_x(0),
	_y(0),
	_z(0) { }

	Vector_3::Vector_3(const double & scalar)
	:
	_x(scalar),
	_y(scalar),
	_z(scalar) { }

	Vector_3::Vector_3(const double & x, const double & y, const double & z)
	:
	_x(x),
	_y(y),
	_z(z) { }

	Vector_3::Vector_3(const double * coordinates)
	:
	_x(coordinates[0]),
	_y(coordinates[1]),
	_z(coordinates[2]) { }

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Getters">

	double Vector_3::x() const
	{
		return _x;
	}

	double Vector_3::y() const
	{
		return _y;
	}

	double Vector_3::z() const
	{
		return _z;
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Unary Operators">

	Vector_3 Vector_3::operator +() const
	{
		return Vector_3(_x, _y, _z);
	}

	Vector_3 Vector_3::operator -() const
	{
		return Vector_3(-_x, -_y, -_z);
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Binary Operators">

	// <editor-fold defaultstate="collapsed" desc="Simple Assignment">

	Vector_3 & Vector_3::operator =(const double & rhs)
	{
		_x = rhs;
		_y = rhs;
		_z = rhs;

		return (* this);
	}

	Vector_3 & Vector_3::operator =(const double * rhs)
	{
		_x = rhs[0];
		_y = rhs[1];
		_z = rhs[2];

		return (* this);
	}

	Vector_3 & Vector_3::operator =(const Vector_3 & rhs)
	{
		if (this != (& rhs)) {
			_x = rhs._x;
			_y = rhs._y;
			_z = rhs._z;
		}

		return (* this);
	}

	// </editor-fold>

	// <editor-fold defaultstate="collapsed" desc="Compound Assignment">

	Vector_3 & Vector_3::operator +=(const double & rhs)
	{
		_x += rhs;
		_y += rhs;
		_z += rhs;

		return (* this);
	}

	Vector_3 & Vector_3::operator -=(const double & rhs)
	{
		_x -= rhs;
		_y -= rhs;
		_z -= rhs;

		return (* this);
	}

	Vector_3 & Vector_3::operator *=(const double & rhs)
	{
		_x *= rhs;
		_y *= rhs;
		_z *= rhs;

		return (* this);
	}

	Vector_3 & Vector_3::operator /=(const double & rhs)
	{
		if (rhs != 0.0) {
			double inverse = 1.0 / rhs;

			_x *= inverse;
			_y *= inverse;
			_z *= inverse;
		}

		return (* this);
	}

	Vector_3 & Vector_3::operator +=(const Vector_3 & rhs)
	{
		_x += rhs.x();
		_y += rhs.y();
		_z += rhs.z();

		return (* this);
	}

	Vector_3 & Vector_3::operator -=(const Vector_3 & rhs)
	{
		_x -= rhs.x();
		_y -= rhs.y();
		_z -= rhs.z();

		return (* this);
	}

	Vector_3 & Vector_3::operator *=(const Vector_3 & rhs)
	{
		_x *= rhs.x();
		_y *= rhs.y();
		_z *= rhs.z();

		return (* this);
	}

	Vector_3 & Vector_3::operator /=(const Vector_3 & rhs)
	{
		_x /= rhs.x();
		_y /= rhs.y();
		_z /= rhs.z();

		return (* this);
	}

//	Vector_3 & Vector_3::operator *=(const Quaternion & rhs)
//	{
//		Quaternion vector(0, _x, _y, _z), rotation = rhs;
//
//		vector = rotation * vector * rotation.conjugate();
//
//		_x = vector.b();
//		_y = vector.c();
//		_z = vector.d();
//
//		/*Vector_3 e1 = (* this), e2, e3;
//		Vector_3 v0(rhs.b(), rhs.c(), rhs.d());
//
//		e2 = v0.cross_product(e1);
//		e3 = v0.cross_product(e2);
//		e2 *= (2.0 * rhs.a());
//		e3 *= 2.0;
//
//		(* this)  = e1 + e2 + e3;*/
//
//		return (* this);
//	}

	// </editor-fold>

	std::ostream & operator <<(std::ostream & out, const Vector_3 & vector)
	{
		out << "("
				<< vector._x
				<< ", "
				<< vector._y
				<< ", "
				<< vector._z
				<< ")";
		return out;
	}

	// </editor-fold>

	double Vector_3::lenght() const
	{
		return std::sqrt((_x * _x) + (_y * _y) + (_z * _z));
	}

	double Vector_3::lenght_square() const
	{
		return ((_x * _x) + (_y * _y) + (_z * _z));
	}

	bool Vector_3::is_zero_lenght() const
	{
		double lenght = ((_x * _x) + (_y * _y) + (_z * _z));
		return (lenght < (Vector_3::EPSILON * Vector_3::EPSILON));
	}

	void Vector_3::normalise()
	{
		double length = ((_x * _x) + (_y * _y) + (_z * _z));

		if (0.0 == length) {
			return;
		}

		double inverse_length = 1.0 / std::sqrt(length);

		_x *= inverse_length;
		_y *= inverse_length;
		_z *= inverse_length;
	}

	double Vector_3::dot_product(const Vector_3 & rhs) const
	{
		return (_x * rhs.x()) + (_y * rhs.y()) + (_z * rhs.z());
	}

	Vector_3 Vector_3::cross_product(const Vector_3 & rhs) const
	{
		double x, y, z;

		x = (_y * rhs.z()) - (_z * rhs.y());
		y = (_z * rhs.x()) - (_x * rhs.z());
		z = (_x * rhs.y()) - (_y * rhs.x());

		return Vector_3(x, y, z);
	}

//	Quaternion Vector_3::calculate_rotation(const Vector_3 & destination) const
//	{
//		// Based on Stan Melax's article in Game Programming Gems
//		double dot;
//		double pi = 4.0 * std::atan(1.0);
//		Vector_3 v0(* this), v1(destination), cross;
//
//		Quaternion result(1, 0, 0, 0);
//
//		v0.normalise();
//		v1.normalise();
//
//		dot = v0.dot_product(v1);
//
//		// If Dot == 1 Vectors Are The Same
//		if (dot >= 1.0) {
//			return Quaternion(1, 0, 0, 0);
//		}
//
//		if (dot < (Vector_3::EPSILON - 1.0)) {
//			// Generate An Axis
//			cross = v0.cross_product(Vector_3::UNIT_X);
//
//			// Pick Another If Colinear
//			if (cross.is_zero_lenght()) {
//				cross = v0.cross_product(Vector_3::UNIT_Y);
//			}
//
//			cross.normalise();
//
//			result = Vector_4(cross.x(), cross.y(), cross.z(), pi);
//		}
//
//		else {
//			double sin_ht, cos_ht;
//			double w, x, y, z;
//
//			cross = v0.cross_product(v1);
//
//			cos_ht = std::sqrt((1 + dot) * 0.5);
//			sin_ht = std::sqrt((1 - dot) * 0.5);
//			/*cos_t = std::sqrt((1 + dot) * 2);
//			sin_ht = 1 / cos_t;
//			cos_ht = cos_t * 0.5;*/
//
//			w = cos_ht;
//			x = cross.x() * sin_ht;
//			y = cross.y() * sin_ht;
//			z = cross.z() * sin_ht;
//
//			result = Quaternion(w, x, y, z);
//			result.normalise();
//		}
//
//		return result;
//	}

	const double * Vector_3::c_array() const
	{
		return (& _x);
	}

	boost::property_tree::ptree Vector_3::export_to_ptree() const
	{
		boost::property_tree::ptree result;

		result.add<double>("X", _x);
		result.add<double>("Y", _y);
		result.add<double>("Z", _z);

		return result;
	}

	void Vector_3::import_from_ptree(const boost::property_tree::ptree & root)
	{
		_x = root.get<double>("X");
		_y = root.get<double>("Y");
		_z = root.get<double>("Z");
	}

	Vector_3 operator +(const double & lhs, const Vector_3 & rhs)
	{
		Vector_3 result(lhs);
		result += rhs;
		return (result);
	}

	Vector_3 operator -(const double & lhs, const Vector_3 & rhs)
	{
		Vector_3 result(lhs);
		result -= rhs;
		return (result);
	}

	Vector_3 operator *(const double & lhs, const Vector_3 & rhs)
	{
		Vector_3 result(lhs);
		result *= rhs;
		return (result);
	}

	Vector_3 operator /(const double & lhs, const Vector_3 & rhs)
	{
		Vector_3 result(lhs);
		result /= rhs;
		return (result);
	}

	Vector_3 operator +(const Vector_3 & lhs, const double & rhs)
	{
		Vector_3 result(lhs);
		result += rhs;
		return (result);
	}

	Vector_3 operator -(const Vector_3 & lhs, const double & rhs)
	{
		Vector_3 result(lhs);
		result -= rhs;
		return (result);
	}

	Vector_3 operator *(const Vector_3 & lhs, const double & rhs)
	{
		Vector_3 result(lhs);
		result *= rhs;
		return (result);
	}

	Vector_3 operator /(const Vector_3 & lhs, const double & rhs)
	{
		Vector_3 result(lhs);
		result /= rhs;
		return (result);
	}

	Vector_3 operator +(const Vector_3 & lhs, const Vector_3 & rhs)
	{
		Vector_3 result(lhs);
		result += rhs;
		return (result);
	}

	Vector_3 operator -(const Vector_3 & lhs, const Vector_3 & rhs)
	{
		Vector_3 result(lhs);
		result -= rhs;
		return result;
	}

	Vector_3 operator *(const Vector_3 & lhs, const Vector_3 & rhs)
	{
		Vector_3 result(lhs);
		result *= rhs;
		return result;
	}

	Vector_3 operator /(const Vector_3 & lhs, const Vector_3 & rhs)
	{
		Vector_3 result(lhs);
		result /= rhs;
		return result;
	}

	/*Vector_3 operator *(const Quaternion & lhs, const Vector_3 & rhs)
	{
		Vector_3 result(lhs);
		result *= rhs;
		return result;
	}*/

//	Vector_3 operator *(const Vector_3 & lhs, const Quaternion & rhs)
//	{
//		Vector_3 result(lhs);
//		result *= rhs;
//		return result;
//	}

	bool operator ==(const Vector_3 & lhs, const Vector_3 & rhs)
	{
		double abs_1 = std::abs(lhs.x() - rhs.x());
		double abs_2 = std::abs(lhs.y() - rhs.y());
		double abs_3 = std::abs(lhs.z() - rhs.z());

		bool abs_1b = abs_1 <= ((std::abs(lhs.x()) < std::abs(rhs.x()) ? std::abs(rhs.x()) : std::abs(lhs.x())) * Vector_3::EPSILON);
		bool abs_2b = abs_2 <= ((std::abs(lhs.y()) < std::abs(rhs.y()) ? std::abs(rhs.y()) : std::abs(lhs.y())) * Vector_3::EPSILON);
		bool abs_3b = abs_3 <= ((std::abs(lhs.z()) < std::abs(rhs.z()) ? std::abs(rhs.z()) : std::abs(lhs.z())) * Vector_3::EPSILON);

		return (abs_1b && abs_2b && abs_3b);
	}

	bool operator !=(const Vector_3 & lhs, const Vector_3 & rhs)
	{
		return (!(lhs == rhs));
	}

	bool operator<(const Vector_3 & lhs, const Vector_3 & rhs)
	{
		return (
				lhs.z() < rhs.z() ||
				(lhs.z() == rhs.z() && lhs.y() < rhs.y()) ||
				(lhs.z() == rhs.z() && lhs.y() == rhs.y() && lhs.x() < rhs.x())
				);
	}

	bool operator>(const Vector_3 & lhs, const Vector_3 & rhs)
	{
		return (!(lhs < rhs));
	}
}
