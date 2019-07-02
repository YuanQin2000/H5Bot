#ifndef __INFO_UNITS_INFO_H__
#define __INFO_UNITS_INFO_H__

#include <cstdint>
#include <set>
#include "sc2api/sc2_interfaces.h"
#include "Utils/Singleton.h"

using namespace std;

#define GENUS_BIOLOGICAL 0x1
#define GENUS_MECHANICAL 0x2
#define GENUS_PSIONIC    0x4

struct UnitAttribute {
	enum ArmorType {
		NoArmor = 0,
		LightArmor,
		HeavyArmor
	};

	enum AttackTarget {
		LandOnly = 0,
		AirOnly,
		LandAndAir
	};

	struct AttackAttribute {
		int Multiplex;
		int Hit;
		int Scope;
		float Frequency;
		AttackTarget Target;
	};

	UnitAttribute()
		: HP(1), EP(0), AntiHit(0), Armor(LightArmor),
		  Genus(GENUS_BIOLOGICAL), Speed(1), Attack(), CargoSize(1) {}

	int HP;	// Health Point
	int EP;	// Energy Point
	int AntiHit;
	ArmorType Armor;
	uint8_t Genus;
	float Speed;
	AttackAttribute Attack;
	int CargoSize;
};

struct UnitBuildCost {
	UnitBuildCost()
		: Mineral(100), Vespene(0), Food(1), Time(100) {}

	int Mineral;
	int Vespene;
	int Food;
	int Time;
};

class CUnitsInfo : public CSingleton<CUnitsInfo>
{
public:
	static const set<sc2::UnitTypeID> s_AvailableUnitsType;

	struct MetaData {
		UnitAttribute Attr;
		UnitBuildCost Cost;
		sc2::AbilityID BuildID;
		vector<sc2::UnitTypeID> ToBuildUnits;
		vector<sc2::UnitTypeID>	RequiredUnits;
	};

public:
	~CUnitsInfo() {}

	void Initialize(const sc2::ObservationInterface* pObj);

	const shared_ptr<MetaData> GetMetaData(sc2::UnitTypeID id);
	void OnTechUpgraded(sc2::UpgradeID id);
	void Dump();

private:
	CUnitsInfo() : m_pQuery(NULL), m_UnitsData() {}

	static const sc2::UnitTypeID s_AvailableUnitsTypeInitArray[];
	static const shared_ptr<MetaData> s_pDefaultMetaData;	// Default value for not found

private:
	const sc2::ObservationInterface* m_pQuery;
	map<sc2::UnitTypeID, shared_ptr<MetaData> > m_UnitsData;
};

#endif