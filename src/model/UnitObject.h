#ifndef __MODEL_UNIT_OBJECT_H__
#define __MODEL_UNIT_OBJECT_H__

#include "sc2api/sc2_unit.h"
#include "Utils/Trace.h"

class sc2::Unit;
class sc2::ActionInterface;
class IUoM;

class CUnitObject
{
public:
	CUnitObject(const sc2::Unit* unit, sc2::ActionInterface* pAction) :
		m_pUnit(unit), m_pSupervisor(NULL), m_pAction(pAction)
	{
		ASSERT(unit);
		ASSERT(pAction);
	}

	virtual ~CUnitObject() {}

	const sc2::Unit* RawUnit() const { return m_pUnit; }
	sc2::ActionInterface* Action() { return m_pAction; }
	IUoM* UoM() { return m_pSupervisor; }
	bool SetUoM(IUoM* mgr);

private:
	const sc2::Unit* m_pUnit;
	IUoM* m_pSupervisor;	// Which UoM does this unit belong
	sc2::ActionInterface* m_pAction;
};

#endif