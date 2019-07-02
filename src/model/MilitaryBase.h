#ifndef __MODEL_MILITARY_BASE_H__
#define __MODEL_MILITARY_BASE_H__

#include <map>
#include <vector>
#include <list>
#include "UoM.h"
#include "Utils/Trace.h"
#include "sc2api/sc2_unit.h"

class CH5Bot;
class CTroop;
class CUnitObject;

using namespace std;

class CMilitaryBase : public IUoM
{
public:
	enum Build_Result {
		BR_OK = 0,
		BR_LACK_MINERAL,
		BR_LACK_VESPENE,
		BR_LACK_TECH,
		BR_LACK_SUPPLY,
		BR_NO_POSITION,
		BR_BUSY
	};

	enum CommandID {
		// SCV to execute construction
		CID_CONSTRUCT_MIN = -1,
		CID_CONSTRUCT_SUPPLYDEPOT,
		CID_CONSTRUCT_BARRACKS,
		CID_CONSTRUCT_REFINERY,
		CID_CONSTRUCT_FACTORY,
		CID_CONSTRUCT_ENGINEERINGBAY,
		CID_CONSTRUCT_MISSILETURRET,
		CID_CONSTRUCT_BUNKER,
		CID_CONSTRUCT_SENSORTOWER,
		CID_CONSTRUCT_GHOSTACADEMY,
		CID_CONSTRUCT_STARPORT,
		CID_CONSTRUCT_COMMANDCENTER,
		CID_CONSTRUCT_MAX,

		// Trainning army/SCV
		CID_TRAINNING_MIN,
		CID_TRAINNING_SCV = 0,
		CID_TRAINNING_MARINE,
		CID_TRAINNING_MARAUDER,
		CID_TRAINNING_REAPER,
		CID_TRAINNING_SIEGETANK,
		CID_TRAINNING_GHOST,
		CID_TRAINNING_BANSHEE,
		CID_TRAINNING_MAX,

		// Upgrade structures.
		CID_UPGRADE_MIN,
		CID_UPGRADE_MAX
	};

	struct ConstructCommand {
		sc2::UnitTypeID BuilderID;
		sc2::UnitTypeID TargetID;
		sc2::AbilityID CmdID;
	};

	struct CommandObject {
		CommandObject(CommandID id, CMilitaryBase* pMB = NULL, int pri = 0) :
			Id(id), pPreferredMB(pMB), Priority(pri) {}

		CommandID Id;
		CMilitaryBase* pPreferredMB;
		int Priority;
	};

	static const ConstructCommand s_AllConstructions[];
	static const sc2::UnitTypeID s_CommandExecutiveUnits[];

public:
	void OnUnitIdle(CUnitObject* unit);
	void OnUnitDestroyed(CUnitObject* unit);

	void OnEnemyEnterVision(const sc2::Unit* unit);

public:
	CMilitaryBase(
		CUnitObject* pCC,
		const vector<CUnitObject*>& scv,
		const vector<CUnitObject*>& mineral,
		const vector<CUnitObject*>& gas,
		map<int, list<CommandObject> >& cmdQueue);
	~CMilitaryBase();

	CTroop* SetSecurityGuard(CTroop* pForce);

	float LaborIndex();
	void AddSCV(CUnitObject* unit);
	void AddMineral(CUnitObject* unit);
	void AddVespene(CUnitObject* unit);

	void NotifyCommand();

	static bool AvailableCommand(CommandID id);
	static const ConstructCommand* GetCommand(CommandID id);

private:
	struct IdleBuilder {
		CUnitObject* pBuilder;
		list<CUnitObject*>* pFrom;
		list<CUnitObject*>* pTo;
	};

	void FindIdleBuilder(sc2::UnitTypeID builder, IdleBuilder* pOut);
	void FindLowPrioritySCV(IdleBuilder* pOut);

	void HandleCommand(list<CommandObject>& cmd);
	Build_Result CMilitaryBase::ExecuteCommand(CommandID id);
	bool CalcBestPosition(sc2::UnitTypeID structureID, sc2::Point2D* pOut);

private:
	CTroop* m_pSecurityGarde;
	CUnitObject* m_pCommandCenters;

	vector<CUnitObject*> m_Minerals;
	vector<CUnitObject*> m_Vespenes;

	map<int, list<CommandObject> >& m_CommandQueue;

	list<CUnitObject*> m_MineralWorkers;
	list<CUnitObject*> m_VespeneWorkers;
	list<CUnitObject*> m_BuildWorkers;
	list<CUnitObject*> m_OtherWorkers;
	list<CUnitObject*> m_IdleWorkers;
	list<CUnitObject*> m_IdleCommandCenters;
	list<CUnitObject*> m_IdleBarracks;
	list<CUnitObject*> m_IdleFactories;
	list<CUnitObject*> m_IdleStarports;
};

#endif