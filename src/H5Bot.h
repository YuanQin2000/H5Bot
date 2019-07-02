#pragma once

#include <map>
#include "sc2api/sc2_agent.h"

using namespace std;

class IUoM;
class CUnitObject;
class CMissionManager;
class CMilitaryBaseManager;
class CIntelligentAgency;

class CH5Bot : public sc2::Agent
{
public:
	virtual void OnGameStart() override;
	virtual void OnStep() override;
    virtual void OnUnitIdle(const sc2::Unit *unit) override;
	virtual void OnUnitDestroyed(const sc2::Unit *unit) override;
    virtual void OnUnitCreated(const sc2::Unit *unit) override;
    virtual void OnUnitEnterVision(const sc2::Unit *unit) override;
	virtual void OnBuildingConstructionComplete(const sc2::Unit *unit) override;
	virtual void OnUpgradeCompleted(sc2::UpgradeID id) override;

public:
	CH5Bot();
	~CH5Bot();

	CMissionManager& TroopManager() { return *m_pMissionMgr; }
	CMilitaryBaseManager& MilitaryBaseManager() { return *m_pMilitaryBaseMgr; }

private:
	CUnitObject* GetUnitObject(const sc2::Unit* unit, bool erase = false);
	void HandleInitialUnits();

private:
	vector<IManager*> m_Managers;

	CIntelligentAgency* m_pIA;
	CMissionManager* m_pMissionMgr;
	CMilitaryBaseManager* m_pMilitaryBaseMgr;

	map<sc2::Tag, CUnitObject*> m_SelfUnits;	// Owned
};

