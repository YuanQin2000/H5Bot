#include "IntelligentAgency.h"
#include "sc2api/sc2_unit.h"
#include "sc2api/sc2_typeenums.h"

CIntelligentAgency::CIntelligentAgency() :
	m_Forces(),
	m_EnemyUnits()
{

}

CIntelligentAgency::~CIntelligentAgency()
{

}

void CIntelligentAgency::OnUnitCreated(const sc2::Unit* unit)
{
	if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_SCV) {
		// Ignore SCV
		return;
	}

	// TODO: Store the force unit (have weapon)
}

void CIntelligentAgency::OnEnemyEnterVision(const sc2::Unit* unit)
{

}

void CIntelligentAgency::OnEnemyDestroyed(const sc2::Unit* unit)
{

}