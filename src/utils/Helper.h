#ifndef __UTILS_HELPER_H__
#define __UTILS_HELPER_H__

#include <vector>

using namespace std;

class sc2::Unit;
struct sc2::UnitTypeData;
struct sc2::Weapon;

namespace NSHelper {

const vector<sc2::UnitTypeID>& GetTownHallTypes();
const vector<sc2::UnitTypeID>& GetMineralTypes();

void PrintUnit(const sc2::Unit* unit);
void PrintUnitTypeData(const sc2::UnitTypeData* pData);
void PrintWeapon(const sc2::Weapon* pWeapon);

}

#endif
