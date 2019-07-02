#ifndef __MODEL_MILITARY_BASE_MANAGER_H__
#define __MODEL_MILITARY_BASE_MANAGER_H__

#include <vector>
#include <map>
#include <list>
#include "UoM.h"
#include "MilitaryBase.h"

class CH5Bot;
class sc2::Unit;

using namespace std;

class CMilitaryBaseManager : public IManager
{
public:
	typedef list<CMilitaryBase::CommandObject> CommandList;

public:
	void OnStep();

	void OnUnitIdle(CUnitObject* unit);
	void OnUnitDestroyed(CUnitObject* unit);
	void OnEnemyEnterVision(const sc2::Unit* unit);

public:
	CMilitaryBaseManager(const sc2::ObservationInterface* pObs);
	~CMilitaryBaseManager();

	void HandleBuildingConstructionComplete(CUnitObject* unit);
	void SetInitialMilitaryBase(
		CUnitObject* pCC,
		const vector<CUnitObject*>& scv,
		const vector<CUnitObject*>& mineral,
		const vector<CUnitObject*>& gas);

private:
	void PushCommand(CMilitaryBase::CommandID id,
		CMilitaryBase* pPreferred = NULL, int priority = 0);
	void CancelCommand(CMilitaryBase::CommandID id);
	void CancelAll() { m_CommandQueue.clear(); }
	void NotifyCommand();

	bool TryExpandMilitaryBase();

private:
	const sc2::ObservationInterface* m_pObs;

	map<int, CommandList> m_CommandQueue; // Build command queue
	vector<CMilitaryBase*> m_MilitaryBases;	// Own
	int m_CheckCommandsCount;
};
#endif