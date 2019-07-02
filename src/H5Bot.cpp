#include <cstdio>
#include "sc2api/sc2_api.h"
#include "sc2api/sc2_interfaces.h"
#include "H5Bot.h"
#include "utils/Helper.h"
#include "utils/Trace.h"
#include "info/UnitsInfo.h"
#include "info/Economy.h"
#include "model/UnitObject.h"
#include "model/UoM.h"
#include "model/MilitaryBaseManager.h"
#include "model/MissionManager.h"
#include "model/IntelligentAgency.h"

using namespace std;

CH5Bot::CH5Bot() :
	m_Managers(),
	m_pIA(NULL),
	m_pMissionMgr(NULL),
	m_pMilitaryBaseMgr(NULL),
	m_SelfUnits()
{
}

CH5Bot::~CH5Bot()
{
	// TODO: release all resources.
}

void CH5Bot::OnGameStart()
{
	CEconomy& eco = CEconomy::Instance();
	eco.Initialize(Observation());
	CUnitsInfo& info = CUnitsInfo::Instance();
	info.Initialize(Observation());

	const sc2::ObservationInterface& query(*Observation());
	m_pIA = new CIntelligentAgency();
	m_pMissionMgr = new CMissionManager();
	m_pMilitaryBaseMgr = new CMilitaryBaseManager(*this, query);

	m_Managers.push_back(m_pMilitaryBaseMgr);
	m_Managers.push_back(m_pMissionMgr);

	HandleInitialUnits();
}

void CH5Bot::OnStep()
{
	for (vector<IManager*>::iterator it = m_Managers.begin();
		it != m_Managers.end(); ++it) {
		IManager* pMgr = *it;
		ASSERT(pMgr);
		pMgr->OnStep();
	}
}

void CH5Bot::OnUnitDestroyed(const sc2::Unit *unit)
{
	if (unit->alliance != sc2::Unit::Self) {
		if (unit->alliance == sc2::Unit::Enemy) {
			m_pIA->OnEnemyDestroyed(unit);
		}
		return;
	}

	CUnitObject* pObj = GetUnitObject(unit, true);
	if (pObj) {
		if (pObj->UoM()) {
			pObj->UoM()->OnUnitDestroyed(pObj);
		} else {
			for (vector<IManager*>::iterator it = m_Managers.begin();
				it != m_Managers.end(); ++it) {
				IUoM* pMgr = *it;
				ASSERT(pMgr);
				pMgr->OnUnitDestroyed(pObj);
			}
		}
		delete pObj;
	}
}

void CH5Bot::OnUnitEnterVision(const sc2::Unit *unit)
{
	switch (unit->alliance) {
	case sc2::Unit::Neutral:
		break;
	case sc2::Unit::Enemy:
		for (vector<IManager*>::iterator it = m_Managers.begin();
			it != m_Managers.end(); ++it) {
			ASSERT(*it);
			(*it)->OnEnemyEnterVision(unit);
		}
		m_pIA->OnEnemyEnterVision(unit);
		return;
		break;
	default:
		break;
	}
}

void CH5Bot::OnBuildingConstructionComplete(const sc2::Unit* unit)
{
	TRACK_FUNCTION_LIFE_CYCLE;
	NSHelper::PrintUnit(unit);

	if (unit->alliance != sc2::Unit::Self) {
		return;
	}

	CUnitObject* pObj = GetUnitObject(unit, true);
	if (pObj) {
		m_pMilitaryBaseMgr->HandleBuildingConstructionComplete(pObj);
	}
}

void CH5Bot::OnUpgradeCompleted(sc2::UpgradeID id)
{
	CUnitsInfo::Instance().OnTechUpgraded(id);
}

void CH5Bot::OnUnitCreated(const sc2::Unit *unit)
{
	TRACK_FUNCTION_LIFE_CYCLE;
	NSHelper::PrintUnit(unit);

	if (unit->alliance != sc2::Unit::Self) {
		return;
	}
	m_pIA->OnUnitCreated(unit);
}

void CH5Bot::OnUnitIdle(const sc2::Unit *unit)
{
	TRACK_FUNCTION_LIFE_CYCLE;
	NSHelper::PrintUnit(unit);

	if (unit->alliance != sc2::Unit::Self) {
		return;
	}

	CUnitObject* pObj = GetUnitObject(unit);
	if (pObj) {
		if (pObj->UoM()) {
			pObj->UoM()->OnUnitIdle(pObj);
			return;
		}
		for (vector<IManager*>::iterator it = m_Managers.begin();
			it != m_Managers.end(); ++it) {
			IManager* pMgr = *it;
			ASSERT(pMgr);
			pMgr->OnUnitIdle(pObj);
		}
	}
}

CUnitObject* CH5Bot::GetUnitObject(const sc2::Unit *unit, bool erased /* = false */)
{
	ASSERT(unit->alliance == sc2::Unit::Self || unit->alliance == sc2::Unit::Neutral);

	CUnitObject* obj = NULL;
	map<sc2::Tag, CUnitObject*>::iterator it = m_SelfUnits.find(unit->tag);
	if (it == m_SelfUnits.end()) {
		obj = new CUnitObject(unit, Actions());
		pair<map<sc2::Tag, CUnitObject*>::iterator, bool> res =
			m_SelfUnits.insert(pair<sc2::Tag, CUnitObject*>(unit->tag, obj));
		if (!res.second) {
			delete obj;
			obj = NULL;
			OUTPUT_WARNING_TRACE("Can not insert new unit!! We have to ignore this unit\n");
			NSHelper::PrintUnit(unit);
		}
	} else {
		// Sanity check
		obj = it->second;
		ASSERT(obj);
		ASSERT(obj->RawUnit() == unit);
		if (erased) {
			m_SelfUnits.erase(it);
		}
	}
	return obj;
}

void CH5Bot::HandleInitialUnits()
{
	vector<CUnitObject*> initialSCV;
	vector<CUnitObject*> mineral;
	vector<CUnitObject*> gas;
	CUnitObject* pinitialCC = NULL;

	initialSCV.reserve(12);
	for (const auto & unit : Observation()->GetUnits()) {
		if (unit->alliance == sc2::Unit::Enemy) {
			m_pIA->OnEnemyEnterVision(unit);
			continue;
		}

		if (unit->alliance == sc2::Unit::Neutral) {
			if (unit->display_type == sc2::Unit::Visible) {
				CUnitObject* pObj = NULL;
				// Handle current minerals
				switch (int(unit->unit_type)) {
				case int(sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER):
				case int(sc2::UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER):
				case int(sc2::UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER):
				case int(sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER):
				case int(sc2::UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER):
					pObj = GetUnitObject(unit);
					ASSERT(pObj);
					gas.push_back(pObj);
					break;

				case int(sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750):
				case int(sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD):
				case int(sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD):
				case int(sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD750) :
				case int(sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD) :
				case int(sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750) :
				case int(sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD) :
				case int(sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750) :
				case int(sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD) :
				case int(sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD750) :
				case int(sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD) :
				case int(sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750) :
					pObj = GetUnitObject(unit);
					ASSERT(pObj);
					mineral.push_back(pObj);
					break;

				default:
					break;
				}
			} else {
				// Handle the potential minerals
			}
			continue;
		}

		// sc2::Unit::Self
		if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_SCV) {
			CUnitObject* pObj = GetUnitObject(unit);
			ASSERT(pObj);
			initialSCV.push_back(pObj);
		} else if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER) {
			ASSERT(pinitialCC == NULL);
			pinitialCC = GetUnitObject(unit);
		}
	}

	ASSERT(m_pMilitaryBaseMgr != NULL);
	ASSERT(pinitialCC != NULL);
	m_pMilitaryBaseMgr->SetInitialMilitaryBase(pinitialCC, initialSCV, mineral, gas);
}