#ifndef __INFO_ECONOMY_H__
#define __INFO_ECONOMY_H__

#include "Utils/Singleton.h"

#define FOOT_CAP_LIMIT 100

class sc2::ObservationInterface;

class CEconomy : public CSingleton<CEconomy>
{
public:
	~CEconomy() {}

	void Initialize(const sc2::ObservationInterface* pObj);

	int Mineral() const;
	int Vespene() const;
	int SupplyCap() const;
	int SupplyUsed() const;
	int WorkerCount() const;
	int ArmyCount() const;
	float DependencyRatio() const;

protected:
	CEconomy() : m_pStatus(nullptr) {}

private:
	const sc2::ObservationInterface* m_pStatus;
};

#endif