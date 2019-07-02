#include "Troop.h"
#include "UnitObject.h"
#include "MissionManager.h"
#include "sc2api/sc2_unit.h"

CTroop::CTroop(CMissionManager& mgr)
	: m_Supervisor(mgr)
{

}

CTroop::~CTroop()
{

}

void CTroop::OnStep()
{

}

void CTroop::OnUnitIdle(CUnitObject* unit)
{

}

void CTroop::OnUnitDestroyed(CUnitObject* unit)
{

}

void CTroop::OnEnemyEnterVision(sc2::Unit* unit)
{
}

void CTroop::Merge(CTroop* pTroop, MissionType type)
{

}