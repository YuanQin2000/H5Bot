#include "Economy.h"
#include "model/UnitObject.h"
#include "utils/Trace.h"
#include "sc2api/sc2_interfaces.h"

CEconomy& CEconomy::Initialize(const sc2::ObservationInterface* pObj)
{
	ASSERT(m_pStatus == NULL);
	ASSERT(pObj);

	m_pStatus = pObj;
}

int CEconomy::Mineral() const
{
	int num = m_pStatus->GetMinerals();
	if (num > 0) {
		return num;
	}
	return 0;
}

int CEconomy::Vespene() const
{
	int num = m_pStatus->GetVespene();
	if (num > 0) {
		return num;
	}
	return 0;
}

int CEconomy::SupplyCap() const
{
	int num = m_pStatus->GetFoodCap();
	if (num > 0) {
		return num;
	}
	return 0;
}

int CEconomy::SupplyUsed() const
{
	int num = m_pStatus->GetFoodUsed();
	if (num > 0) {
		return num;
	}
	return 0;
}

int CEconomy::WorkerCount() const
{
	int num = m_pStatus->GetFoodWorkers();
	if (num > 0) {
		return num;
	}
	return 0;
}

int CEconomy::ArmyCount() const
{
	int num = m_pStatus->GetArmyCount();
	if (num > 0) {
		return num;
	}
	return 0;
}

float CEconomy::DependencyRatio() const
{
	unsigned int armies = ArmyCount();
	unsigned int workers = WorkerCount();
	if (workers != 0) {
		return float(armies) / float(workers);
	}
	return 0;
}
