#ifndef __MODEL_UOM_H__
#define __MODEL_UOM_H__

#include "sc2api/sc2_unit.h"

class CUnitObject;

class IUoM
{
public:
	virtual void OnUnitIdle(CUnitObject* unit) = 0;
	virtual void OnUnitDestroyed(CUnitObject* unit) = 0;
	virtual void OnEnemyEnterVision(const sc2::Unit* unit) = 0;
};

class IManager : public IUoM
{
public:
	virtual void OnStep() = 0;
};

#endif