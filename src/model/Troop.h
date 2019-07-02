#ifndef __MODEL_TROOP_H__
#define __MODEL_TROOP_H__

#include "UoM.h"

class CUnitObject;
class sc2::Unit;
class CMissionManager;

class CTroop : public IUoM
{
public:
	enum MissionType {
		Scout = 0,
		Combat,
		SecurityGuard
	};

public:
	void OnStep();

	void OnUnitIdle(CUnitObject* unit);
	void OnUnitDestroyed(CUnitObject* unit);

	void OnEnemyEnterVision(sc2::Unit* unit);

public:
	CTroop(CMissionManager& mgr);
	~CTroop();

	void Merge(CTroop* pTroop, MissionType type);

private:
	CMissionManager& m_Supervisor;
};

#endif