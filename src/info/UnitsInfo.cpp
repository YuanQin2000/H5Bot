#include "UnitsInfo.h"
#include "utils/Helper.h"
#include "utils/Trace.h"
#include "sc2api/sc2_interfaces.h"

const sc2::UnitTypeID CUnitsInfo::s_AvailableUnitsTypeInitArray[] = {
	// All Terran
	sc2::UNIT_TYPEID::TERRAN_BANSHEE, // force, OK
	sc2::UNIT_TYPEID::TERRAN_MARAUDER, // force, OK
	sc2::UNIT_TYPEID::TERRAN_MARINE, // force, OK
	sc2::UNIT_TYPEID::TERRAN_GHOST, // force, OK
	sc2::UNIT_TYPEID::TERRAN_REAPER, // force, OK
	sc2::UNIT_TYPEID::TERRAN_SIEGETANK, // force, OK
	sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED, // force, OK

	sc2::UNIT_TYPEID::TERRAN_BARRACKS,
	sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING,
	sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR,
	sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB,
	sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER,
	sc2::UNIT_TYPEID::TERRAN_BUNKER,
	sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER,
	sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING,
	sc2::UNIT_TYPEID::TERRAN_CYCLONE,
	sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY,
	sc2::UNIT_TYPEID::TERRAN_FACTORY,
	sc2::UNIT_TYPEID::TERRAN_FACTORYFLYING,
	sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR,
	sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB,
	sc2::UNIT_TYPEID::TERRAN_FUSIONCORE,
	sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY,
	sc2::UNIT_TYPEID::TERRAN_HELLION,
	sc2::UNIT_TYPEID::TERRAN_HELLIONTANK,
	sc2::UNIT_TYPEID::TERRAN_LIBERATOR,
	sc2::UNIT_TYPEID::TERRAN_LIBERATORAG,
	sc2::UNIT_TYPEID::TERRAN_MEDIVAC,
	sc2::UNIT_TYPEID::TERRAN_MISSILETURRET,
	sc2::UNIT_TYPEID::TERRAN_MULE,
	sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND,
	sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING,
	sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS,
	sc2::UNIT_TYPEID::TERRAN_RAVEN,
	sc2::UNIT_TYPEID::TERRAN_REFINERY,
	sc2::UNIT_TYPEID::TERRAN_SCV,
	sc2::UNIT_TYPEID::TERRAN_SENSORTOWER,
	sc2::UNIT_TYPEID::TERRAN_STARPORT,
	sc2::UNIT_TYPEID::TERRAN_STARPORTFLYING,
	sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR,
	sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB,
	sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT,
	sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED,
	sc2::UNIT_TYPEID::TERRAN_THOR,
	sc2::UNIT_TYPEID::TERRAN_THORAP,
	sc2::UNIT_TYPEID::TERRAN_VIKINGASSAULT,
	sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER,
	sc2::UNIT_TYPEID::TERRAN_WIDOWMINE,
	sc2::UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED,

	// Terran non-interactive
	sc2::UNIT_TYPEID::TERRAN_KD8CHARGE,
	sc2::UNIT_TYPEID::TERRAN_NUKE,
	sc2::UNIT_TYPEID::TERRAN_POINTDEFENSEDRONE,
	sc2::UNIT_TYPEID::TERRAN_REACTOR,
	sc2::UNIT_TYPEID::TERRAN_TECHLAB
};

const set<sc2::UnitTypeID> CUnitsInfo::s_AvailableUnitsType(
	s_AvailableUnitsTypeInitArray,
	s_AvailableUnitsTypeInitArray + sizeof(s_AvailableUnitsTypeInitArray) / sizeof(s_AvailableUnitsTypeInitArray[0])
);

const shared_ptr<CUnitsInfo::MetaData> CUnitsInfo::s_pDefaultMetaData(new MetaData());

void CUnitsInfo::Initialize(const sc2::ObservationInterface* query)
{
	ASSERT(m_pQuery == NULL);

	const sc2::UnitTypes& unitTypes = m_pQuery->GetUnitTypeData(true);
	for (sc2::UnitTypes::const_iterator it = unitTypes.begin();
		it != unitTypes.end(); ++it) {
		sc2::UnitTypeData typeData = *it;
		if (s_AvailableUnitsType.find(typeData.unit_type_id) != s_AvailableUnitsType.end()) {
			// available unit. Store it.
			MetaData* pMeta = new MetaData();

			// Attribution: TODO
			pMeta->Attr.Speed = typeData.movement_speed;
			pMeta->Attr.CargoSize = typeData.cargo_size;

			switch (sc2::UNIT_TYPEID(typeData.unit_type_id)) {
			case sc2::UNIT_TYPEID::TERRAN_BANSHEE:
				pMeta->Attr.HP = 140;
				pMeta->Attr.EP = 200;
				break;
			case sc2::UNIT_TYPEID::TERRAN_SCV:
				pMeta->Attr.HP = 45;
				pMeta->Attr.EP = 0;
				break;
			case sc2::UNIT_TYPEID::TERRAN_MARINE:
				pMeta->Attr.HP = 45;
				pMeta->Attr.EP = 0;
				break;
			case sc2::UNIT_TYPEID::TERRAN_MARAUDER:
				pMeta->Attr.HP = 125;
				pMeta->Attr.EP = 0;
				break;
			case sc2::UNIT_TYPEID::TERRAN_REAPER:
				pMeta->Attr.HP = 60;
				pMeta->Attr.EP = 0;
				break;
			case sc2::UNIT_TYPEID::TERRAN_GHOST:
				pMeta->Attr.HP = 100;
				pMeta->Attr.EP = 200;
				break;
			case sc2::UNIT_TYPEID::TERRAN_SIEGETANK:
				pMeta->Attr.HP = 175;
				pMeta->Attr.EP = 0;
				break;
			default:
				break;
			}

			// Build Cost
			pMeta->Cost.Mineral = typeData.mineral_cost;
			pMeta->Cost.Vespene = typeData.vespene_cost;
			pMeta->Cost.Time = int(typeData.build_time);
			pMeta->Cost.Food = int(typeData.food_required);

			pair<map<sc2::UnitTypeID, shared_ptr<MetaData> >::iterator, bool> res =
				m_UnitsData.insert(pair<sc2::UnitTypeID, shared_ptr<MetaData> >(typeData.unit_type_id, pMeta));
			if (res.second == false) {
				OUTPUT_WARNING_TRACE("Insert Attribute data (ID: %d) failed.\n", typeData.unit_type_id);
				delete pMeta;
			}
		}
	}
}

const shared_ptr<CUnitsInfo::MetaData> CUnitsInfo::GetMetaData(sc2::UnitTypeID id)
{
	shared_ptr<CUnitsInfo::MetaData> res = s_pDefaultMetaData;
	map<sc2::UnitTypeID, shared_ptr<CUnitsInfo::MetaData> >::iterator it = m_UnitsData.find(id);
	if (it != m_UnitsData.end()) {
		res = it->second;
	}
	return res;
}

void CUnitsInfo::OnTechUpgraded(sc2::UpgradeID id)
{
	const sc2::UnitTypes& unitTypes = m_pQuery->GetUnitTypeData(true);
	for (sc2::UnitTypes::const_iterator it = unitTypes.begin();
		it != unitTypes.end(); ++it) {
		sc2::UnitTypeData typeData = *it;
		if (typeData.available &&
			s_AvailableUnitsType.find(typeData.unit_type_id) != s_AvailableUnitsType.end()) {
			// available unit. Update it.
			map<sc2::UnitTypeID, shared_ptr<CUnitsInfo::MetaData> >::iterator it
				= m_UnitsData.find(typeData.unit_type_id);
			if (it == m_UnitsData.end()) {
				OUTPUT_WARNING_TRACE("See unknown unit after upgrade: (ID: %d)\n", typeData.unit_type_id);
				continue;
			}

			shared_ptr<CUnitsInfo::MetaData> pMeta = it->second;

			// Attribution: TODO
			pMeta->Attr.Speed = typeData.movement_speed;
			pMeta->Attr.CargoSize = typeData.cargo_size;

			// Build cost
			if (pMeta->Cost.Mineral != typeData.mineral_cost) {
				OUTPUT_NOTICE_TRACE("Different build cost after upgrade: (ID: %d), (Mineral: %d->%d)\n",
					typeData.unit_type_id, pMeta->Cost.Mineral, typeData.mineral_cost);
			}
			if (pMeta->Cost.Vespene != typeData.vespene_cost) {
				OUTPUT_NOTICE_TRACE("Different build cost after upgrade: (ID: %d), (Vespene: %d->%d)\n",
					typeData.unit_type_id, pMeta->Cost.Vespene, typeData.vespene_cost);
			}
			if (pMeta->Cost.Food != typeData.food_required) {
				OUTPUT_NOTICE_TRACE("Different build cost after upgrade: (ID: %d), (Food: %d->%d)\n",
					typeData.unit_type_id, pMeta->Cost.Food, typeData.food_required);
			}
			if (pMeta->Cost.Time != typeData.build_time) {
				OUTPUT_NOTICE_TRACE("Different build cost after upgrade: (ID: %d), (Time: %d->%d)\n",
					typeData.unit_type_id, pMeta->Cost.Time, typeData.build_time);
			}

			pMeta->Cost.Mineral = typeData.mineral_cost;
			pMeta->Cost.Vespene = typeData.vespene_cost;
			pMeta->Cost.Time = unsigned int(typeData.build_time);
			pMeta->Cost.Food = unsigned int(typeData.food_required);
		}
	}
}

void CUnitsInfo::Dump()
{
}
