#include "MissionManager.h"
#include "Troop.h"
#include "UnitObject.h"

CMissionManager::CMissionManager()
{

}

CMissionManager::~CMissionManager()
{

}

void CMissionManager::OnStep()
{

}

void CMissionManager::OnUnitIdle(CUnitObject* unit)
{
	sc2::UnitTypeID unitType = unit->RawUnit()->unit_type;

	if (unitType == sc2::UNIT_TYPEID::TERRAN_BANSHEE
		|| unitType == sc2::UNIT_TYPEID::TERRAN_MARAUDER
		|| unitType == sc2::UNIT_TYPEID::TERRAN_MARINE
		|| unitType == sc2::UNIT_TYPEID::TERRAN_GHOST
		|| unitType == sc2::UNIT_TYPEID::TERRAN_REAPER
		|| unitType == sc2::UNIT_TYPEID::TERRAN_SIEGETANK
		|| unitType == sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED) {
		// TODO:
	}
}

void CMissionManager::OnUnitDestroyed(CUnitObject* unit)
{

}

void CMissionManager::OnEnemyEnterVision(const sc2::Unit* unit)
{

}

