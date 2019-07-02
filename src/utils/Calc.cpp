#include "Calc.h"

namespace NSCalc {
	
float FastSqrt(const float S)
{
	float x = 1.0f;
	while (x * x < S) {
		x += 0.5f;
	}
	for (int j = 0; j < 5; ++j) {
		x = 0.5f * (x + S / x);
	}
	return x;
}

float DistanceSqrt(const sc2::Point2D& p1, const sc2::Point2D& p2)
{
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;

	return dx * dx + dy * dy;
}

sc2::Point2D NormalizeVector(const sc2::Point2D pos, const float length)
{
	if (Distance(pos) == 0.0f) {
		return sc2::Point2D();
	}
	return (length / Distance(pos)) * pos;
}

}