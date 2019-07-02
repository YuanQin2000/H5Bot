#include <cstdio>
#include "sc2api/sc2_unit.h"
#include "sc2api/sc2_data.h"
#include "sc2api/sc2_interfaces.h"
#include "Helper.h"

using namespace std;

namespace NSHelper {

const vector<sc2::UnitTypeID>& GetTownHallTypes()
{
	static const vector<sc2::UnitTypeID> s_TownHalls = {
		sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER,
		sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING,
		sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND,
		sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING,
		sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS};

	return s_TownHalls;
}

const vector<sc2::UnitTypeID>& GetMineralTypes()
{
	static const vector<sc2::UnitTypeID> s_Minerals = {
		sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD,
		sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD750,
		sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD,
		sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750,
		sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD,
		sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD750,
		sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD,
		sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750,
		sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD,
		sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750,
		sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD,
		sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750
	};

	return s_Minerals;
}

static const char* s_DisplayArray[] = {
	"Not Valid",
	"Visible",
	"Snapshot",
	"Hidden"
};

static const char* s_Relationship[] = {
	"Not Valid",
	"Self",
	"Ally",
	"Neutral",
	"Enemy"
};

static const char* s_Attribution[] = {
	"Not valid",
	"Light",
	"Armored",
	"Biological",
	"Mechanical",
	"Robotic",
	"Psionic",
	"Massive",
	"Structure",
	"Hover",
	"Heroic",
	"Summoned",
	"Invalid"
};


void PrintUnit(const sc2::Unit *unit)
{
	printf("--------Unit BEGIN--------\n");
	printf("\t Tag: %llu\n", unit->tag);
	printf("\t Type: %d\n", unit->unit_type);
	printf("\t Relationship: %s\n", s_Relationship[unit->alliance]);
	printf("\t Position: (%f:%f:%f), Radius: %f\n", unit->pos.x, unit->pos.y, unit->pos.z, unit->radius);
	printf("\t Display Type: %s\n", s_DisplayArray[unit->display_type]);
	printf("\t Selected: %s\n", unit->is_selected ? "YES" : "NO");
	printf("\t Health: %f/%f\n", unit->health, unit->health_max);
	printf("\t Shield: %f/%f\n", unit->shield, unit->shield_max);
	printf("\t Energy: %f/%f\n", unit->energy, unit->energy_max);
	printf("\t Flying: %s\n", unit->is_flying ? "YES" : "NO");
	printf("---------Unit END---------\n");
}

void PrintWeapon(const sc2::Weapon* pWeapon)
{
	static const char* s_WeaponTarget[] = {
		"Not valid",
		"Ground",
		"Air",
		"Any",
		"Invalid"
	};

	printf("--------Weapon BEGIN--------\n");
	printf("\t Target: %s\n", s_WeaponTarget[int(pWeapon->type)]);
	printf("\t Damage: %f\n", pWeapon->damage_);
	printf("\t Multiplex: %d\n", pWeapon->attacks);
	printf("\t Range: %f\n", pWeapon->range);
	printf("\t Speed: %f\n", pWeapon->speed);

	printf("\t Bonus:\n");
	for (vector<sc2::DamageBonus>::const_iterator it = pWeapon->damage_bonus.begin();
		it != pWeapon->damage_bonus.end(); ++it) {
		printf("\t\t Attribute: %s, +%f\n", s_Attribution[int(it->attribute)], it->bonus);
	}

	printf("--------Weapon END--------\n");
}

void PrintUnitTypeData(const sc2::UnitTypeData* pData)
{
	printf("--------UnitTypeData BEGIN--------\n");
	printf("\t TypeID: %d\n", int(pData->unit_type_id));
	printf("\t Name: %s\n", pData->name.c_str());
	printf("\t Mineral cost: %d\n", pData->mineral_cost);
	printf("\t Vespene cost: %d\n", pData->vespene_cost);
	printf("\t Food cost: %f\n", pData->food_required);
	printf("\t Time cost: %f\n", pData->build_time);

	for (vector<sc2::Attribute>::const_iterator it = pData->attributes.begin();
		it != pData->attributes.end(); ++it) {
		printf("Attribute: %s\n", s_Attribution[int(*it)]);
	}

	printf("\t Movement speed: %f\n", pData->movement_speed);
	printf("\t Armor (Anti-hit): %f\n", pData->armor);

	for (vector<sc2::Weapon>::const_iterator it = pData->weapons.begin();
		it != pData->weapons.end(); ++it) {
		PrintWeapon(&(*it));
	}

	printf("---------UnitTypeData END---------\n");
}

}