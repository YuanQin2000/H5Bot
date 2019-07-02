#ifndef __MODEL_INTELLIGENCE_AGENCY_H__
#define __MODEL_INTELLIGENCE_AGENCY_H__

#include <map>
#include <list>
#include "sc2api/sc2_unit.h"

using namespace std;

class CIntelligentAgency
{
public:
	CIntelligentAgency();
	~CIntelligentAgency();

	void OnUnitCreated(const sc2::Unit* unit);
	void OnEnemyEnterVision(const sc2::Unit* unit);
	void OnEnemyDestroyed(const sc2::Unit* unit);

private:
	map<sc2::UnitTypeID, list<sc2::Unit*> > m_Forces;
	map<sc2::Tag, sc2::Unit*> m_EnemyUnits;
};

#endif