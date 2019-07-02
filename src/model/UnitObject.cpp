#include "UnitObject.h"
#include "UoM.h"
#include "sc2api/sc2_interfaces.h"

bool CUnitObject::SetUoM(IUoM* mgr)
{
	bool isOK = false;
	if (m_pSupervisor == NULL) {
		m_pSupervisor = mgr;
		isOK = true;
	} else {
		// TODO: Negotiate with the previous UoM.
	}

	return isOK;
}

