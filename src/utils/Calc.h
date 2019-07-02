#ifndef __UTILS_CALC_H__
#define __UTILS_CALC_H__

#include "sc2api/sc2_unit.h"

namespace NSCalc {

float FastSqrt(const float S);
float DistanceSqrt(const sc2::Point2D& p1, const sc2::Point2D& p2);
sc2::Point2D NormalizeVector(const sc2::Point2D pos, const float length);

float Distance(const sc2::Point2D& p1, const sc2::Point2D& p2)
{
	return FastSqrt(DistanceSqrt(p1, p2));
}

float Distance(const sc2::Point2D& p1)
{
	return FastSqrt(DistanceSqrt(p1));
}

float Distance(const sc2::Unit* a, const sc2::Unit* b)
{
	return FastSqrt(DistanceSqrt(a->pos, b->pos)) - a->radius - b->radius;
}

float DistanceSqrt(const sc2::Point2D &p1)
{
	return p1.x * p1.x + p1.y * p1.y;
}

}

#endif