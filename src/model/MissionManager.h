#ifndef __MODEL_MISSION_MANAGER_H__
#define __MODEL_MISSION_MANAGER_H__

#include <vector>
#include "UoM.h"

using namespace std;

class CTroop;
class sc2::Unit;

class CMissionManager : public IManager
{
public:
	void OnStep();

	void OnUnitIdle(CUnitObject* unit);
	void OnUnitDestroyed(CUnitObject* unit);
	void OnEnemyEnterVision(const sc2::Unit* unit);

public:
	CMissionManager();
	~CMissionManager();

private:
	vector<CTroop*> m_pTroops;	// Own
};

#endif