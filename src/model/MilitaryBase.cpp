#include "MilitaryBase.h"
#include "UnitObject.h"
#include "utils/Trace.h"
#include "info/UnitsInfo.h"
#include "info/Economy.h"
#include "H5Bot.h"

const CMilitaryBase::ConstructCommand s_AllConstructions[] = {
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT, sc2::ABILITY_ID::BUILD_SUPPLYDEPOT },
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_BARRACKS, sc2::ABILITY_ID::BUILD_BARRACKS },
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_REFINERY, sc2::ABILITY_ID::BUILD_REFINERY },
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_FACTORY, sc2::ABILITY_ID::BUILD_FACTORY },
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, sc2::ABILITY_ID::BUILD_ENGINEERINGBAY },
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_MISSILETURRET, sc2::ABILITY_ID::BUILD_MISSILETURRET },
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_BUNKER, sc2::ABILITY_ID::BUILD_BUNKER },
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_SENSORTOWER, sc2::ABILITY_ID::BUILD_SENSORTOWER },
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY, sc2::ABILITY_ID::BUILD_GHOSTACADEMY },
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_STARPORT, sc2::ABILITY_ID::BUILD_STARPORT },
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, sc2::ABILITY_ID::BUILD_COMMANDCENTER },
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_ARMORY, sc2::ABILITY_ID::BUILD_ARMORY },
	{ sc2::UNIT_TYPEID::TERRAN_SCV, sc2::UNIT_TYPEID::TERRAN_FUSIONCORE, sc2::ABILITY_ID::BUILD_FUSIONCORE },

	{ sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, sc2::UNIT_TYPEID::TERRAN_SCV, sc2::ABILITY_ID::TRAIN_SCV },
	{ sc2::UNIT_TYPEID::TERRAN_BARRACKS, sc2::UNIT_TYPEID::TERRAN_MARINE, sc2::ABILITY_ID::TRAIN_MARINE },
	{ sc2::UNIT_TYPEID::TERRAN_BARRACKS, sc2::UNIT_TYPEID::TERRAN_MARAUDER, sc2::ABILITY_ID::TRAIN_MARAUDER },
	{ sc2::UNIT_TYPEID::TERRAN_BARRACKS, sc2::UNIT_TYPEID::TERRAN_REAPER, sc2::ABILITY_ID::TRAIN_REAPER },
	{ sc2::UNIT_TYPEID::TERRAN_BARRACKS, sc2::UNIT_TYPEID::TERRAN_GHOST, sc2::ABILITY_ID::TRAIN_GHOST },
	{ sc2::UNIT_TYPEID::TERRAN_FACTORY, sc2::UNIT_TYPEID::TERRAN_SIEGETANK, sc2::ABILITY_ID::TRAIN_SIEGETANK },
	{ sc2::UNIT_TYPEID::TERRAN_STARPORT, sc2::UNIT_TYPEID::TERRAN_BANSHEE, sc2::ABILITY_ID::TRAIN_BANSHEE },
};

CMilitaryBase::CMilitaryBase(
	CUnitObject* pCC,
	const vector<CUnitObject*>& scv,
	const vector<CUnitObject*>& mineral,
	const vector<CUnitObject*>& gas,
	map<int, list<CommandObject> >& cmdQueue) :
	m_pSecurityGarde(NULL),
	m_pCommandCenters(pCC),
	m_Minerals(),
	m_Vespenes(),
	m_CommandQueue(cmdQueue),
	m_MineralWorkers(),
	m_VespeneWorkers(),
	m_BuildWorkers(),
	m_OtherWorkers(),
	m_IdleWorkers(),
	m_IdleCommandCenters(),
	m_IdleBarracks(),
	m_IdleFactories(),
	m_IdleStarports()
{
	for (vector<CUnitObject*>::const_iterator it = scv.begin();
		it != scv.end(); ++it) {
		AddSCV(*it);
	}
	for (vector<CUnitObject*>::const_iterator it = mineral.begin();
		it != mineral.end(); ++it) {
		AddMineral(*it);
	}
	for (vector<CUnitObject*>::const_iterator it = gas.begin();
		it != gas.end(); ++it) {
		AddVespene(*it);
	}
	pCC->SetUoM(this);
}

CMilitaryBase::~CMilitaryBase()
{

}

void CMilitaryBase::OnUnitIdle(CUnitObject* unit)
{
	int type = int(unit->RawUnit()->unit_type);

	switch (type) {
	case int(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER) :
		break;

	case int(sc2::UNIT_TYPEID::TERRAN_BARRACKS) :
		// TODO: build Marine.
		break;
	default:
		break;
	}
}

void CMilitaryBase::OnUnitDestroyed(CUnitObject* unit)
{

}

void CMilitaryBase::OnEnemyEnterVision(const sc2::Unit* unit)
{

}

CTroop* CMilitaryBase::SetSecurityGuard(CTroop* pForce)
{
	CTroop* pTmp = m_pSecurityGarde;
	m_pSecurityGarde = pForce;
	return pTmp;
}

float CMilitaryBase::LaborIndex()
{
	int miners = 0;
	int vespene = 0;
	for (vector<CUnitObject*>::iterator it = m_Minerals.begin();
		it != m_Minerals.end(); ++it) {
		miners += (*it)->RawUnit()->mineral_contents;
	}
	for (vector<CUnitObject*>::iterator it = m_Vespenes.begin();
		it != m_Vespenes.end(); ++it) {
		miners += (*it)->RawUnit()->vespene_contents;
	}

	// TODO:  All workers / (mineral + gas)
	return float(m_MineralWorkers.size()
		+ m_VespeneWorkers.size()
		+ m_BuildWorkers.size()
		+ m_OtherWorkers.size()
		+ m_IdleWorkers.size()) / float(miners + vespene);
}

void CMilitaryBase::AddSCV(CUnitObject* unit)
{
	unit->SetUoM(this);

	if (m_pCommandCenters == NULL) {
		// TODO: do we need build command center?
		return;
	}

	// TODO: Assign a proper work to the SCV
	if (m_MineralWorkers.size() < m_pCommandCenters->RawUnit()->ideal_harvesters) {
		m_MineralWorkers.push_back(unit);

		return;
	}
}

void CMilitaryBase::AddMineral(CUnitObject* unit)
{
	if (unit->RawUnit()->mineral_contents > 0) {
		m_Minerals.push_back(unit);
	}
}

void CMilitaryBase::AddVespene(CUnitObject* unit)
{
	if (unit->RawUnit()->vespene_contents > 0) {
		m_Vespenes.push_back(unit);
	}
}

void CMilitaryBase::NotifyCommand()
{
	for (map<int, list<CommandObject> >::iterator it = m_CommandQueue.begin();
		it != m_CommandQueue.end(); ++it) {
		HandleCommand(it->second);
	}
}

void CMilitaryBase::HandleCommand(list<CommandObject>& cmd)
{
	if (cmd.size() == 0) {
		return;
	}

	for (list<CommandObject>::iterator it = cmd.begin();
		it != cmd.end(); ++it) {
		if (it->pPreferredMB == NULL || it->pPreferredMB == this) {
			if (ExecuteCommand(it->Id) == BR_OK) {
				cmd.erase(it);
			}
			break;
		}
	}
}

CMilitaryBase::Build_Result CMilitaryBase::ExecuteCommand(CommandID id)
{
	// Build Structure
	const ConstructCommand* pCC = &s_AllConstructions[id];
	const shared_ptr<CUnitsInfo::MetaData> metaData = CUnitsInfo::Instance().GetMetaData(pCC->TargetID);
	if (CEconomy::Instance().Mineral() < metaData->Cost.Mineral) {
		return BR_LACK_MINERAL;
	}
	if (CEconomy::Instance().Vespene() < metaData->Cost.Vespene) {
		return BR_LACK_VESPENE;
	}

	// Find a idle builder first
	IdleBuilder ib;
	FindIdleBuilder(pCC->BuilderID, &ib);
	if (ib.pBuilder == NULL) {
		return BR_BUSY;
	}
	sc2::Point2D pos;
	if (!CalcBestPosition(pCC->TargetID, &pos)) {
		return BR_NO_POSITION;
	}
	ib.pBuilder->Action()->UnitCommand(ib.pBuilder->RawUnit(), pCC->CmdID, pos);
	if (ib.pFrom) {
		ib.pFrom->pop_front();
	}
	if (ib.pTo) {
		ib.pTo->push_back(ib.pBuilder);
	}
	return BR_OK;
}

bool CMilitaryBase::AvailableCommand(CommandID id)
{
	return (id > CID_CONSTRUCT_MIN && id < CID_CONSTRUCT_MAX)
		|| (id > CID_TRAINNING_MIN && id < CID_TRAINNING_MAX)
		|| (id > CID_UPGRADE_MIN && id < CID_UPGRADE_MAX);
}

const CMilitaryBase::ConstructCommand* CMilitaryBase::GetCommand(CommandID id)
{
	ASSERT(AvailableCommand(id));
	return &s_AllConstructions[id];
}


void CMilitaryBase::FindIdleBuilder(sc2::UnitTypeID builder, IdleBuilder* pOut)
{
	if (builder == sc2::UNIT_TYPEID::TERRAN_SCV) {
		FindLowPrioritySCV(pOut);
		return;
	}

	CUnitObject* pRes = NULL;
	list<CUnitObject*>* pL = NULL;
	switch (int(builder)) {
	case int(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER) :
		pL = &m_IdleCommandCenters;
		break;
	case int(sc2::UNIT_TYPEID::TERRAN_BARRACKS) :
		pL = &m_IdleBarracks;
		break;
	case int(sc2::UNIT_TYPEID::TERRAN_FACTORY):
		pL = &m_IdleFactories;
		break;
	case int(sc2::UNIT_TYPEID::TERRAN_STARPORT):
		pL = &m_IdleStarports;
		break;
	default:
		break;
	}

	if (pL != NULL && pL->size() > 0) {
		pRes = pL->front();
	}

	pOut->pBuilder = pRes;
	pOut->pFrom = pL;
	pOut->pTo = NULL;
}

void CMilitaryBase::FindLowPrioritySCV(IdleBuilder* pOut)
{
	CUnitObject* pRes = NULL;
	list<CUnitObject*>* pL = NULL;

	if (m_IdleWorkers.size() > 0) {
		pRes = m_IdleWorkers.front();
		pL = &m_IdleWorkers;
	} else if (m_MineralWorkers.size() > 0) {
		pRes = m_MineralWorkers.front();
		pL = &m_MineralWorkers;
	} else if (m_VespeneWorkers.size() > 0) {
		pRes = m_VespeneWorkers.front();
		pL = &m_VespeneWorkers;
	} else if (m_OtherWorkers.size() > 0) {
		pRes = m_OtherWorkers.front();
		pL = &m_OtherWorkers;
	}

	pOut->pBuilder = pRes;
	pOut->pFrom = pL;
	pOut->pTo = &m_BuildWorkers;
}

bool CMilitaryBase::CalcBestPosition(sc2::UnitTypeID structureID, sc2::Point2D* pOut)
{
	// Calc map
	return true;
}