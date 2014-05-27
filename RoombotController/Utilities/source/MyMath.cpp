#include "MyMath.h"

namespace utils
{
	const double MyMath::PI = (4.0 * std::atan(1.0));
	const double MyMath::PI_HALF = (0.5 * MyMath::PI);
	const double MyMath::PI_DOUBLE = (2.0 * MyMath::PI);
	const double MyMath::PI_INVERSE = (1.0 / MyMath::PI);
	const double MyMath::PI_HALF_INVERSE = (1.0 / MyMath::PI_HALF);
	const double MyMath::PI_DOUBLE_INVERSE = (1.0 / MyMath::PI_DOUBLE);
}