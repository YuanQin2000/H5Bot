#include "MilitaryBaseManager.h"

#include "utils/Trace.h"
#include "sc2api/sc2_control_interfaces.h"

CMilitaryBaseManager::CMilitaryBaseManager(const sc2::ObservationInterface* pObs) :
	m_pObs(pObs),
	m_CommandQueue(),
	m_MilitaryBases(),
	m_CheckCommandsCount(0)
{
}

CMilitaryBaseManager::~CMilitaryBaseManager()
{

}

void CMilitaryBaseManager::OnStep()
{
	if (++m_CheckCommandsCount = 10) {
		NotifyCommand();
	}
}

void CMilitaryBaseManager::OnUnitIdle(CUnitObject* unit)
{

}

void CMilitaryBaseManager::OnUnitDestroyed(CUnitObject* unit)
{

}

void CMilitaryBaseManager::OnEnemyEnterVision(const sc2::Unit* unit)
{

}

void CMilitaryBaseManager::HandleBuildingConstructionComplete(CUnitObject* unit)
{

}

void CMilitaryBaseManager::SetInitialMilitaryBase(
	CUnitObject* pCC,
	const vector<CUnitObject*>& scv,
	const vector<CUnitObject*>& mineral,
	const vector<CUnitObject*>& gas)
{
	PushCommand(CMilitaryBase::CID_TRAINNING_SCV);
	PushCommand(CMilitaryBase::CID_CONSTRUCT_SUPPLYDEPOT);
	PushCommand(CMilitaryBase::CID_CONSTRUCT_BARRACKS);
	PushCommand(CMilitaryBase::CID_TRAINNING_MARINE);
	PushCommand(CMilitaryBase::CID_TRAINNING_MARINE);
	PushCommand(CMilitaryBase::CID_TRAINNING_MARINE);
	PushCommand(CMilitaryBase::CID_TRAINNING_MARINE);
	PushCommand(CMilitaryBase::CID_TRAINNING_MARINE);
	PushCommand(CMilitaryBase::CID_CONSTRUCT_REFINERY);
	PushCommand(CMilitaryBase::CID_CONSTRUCT_REFINERY);
	PushCommand(CMilitaryBase::CID_CONSTRUCT_SUPPLYDEPOT);
	m_MilitaryBases.push_back(new CMilitaryBase(pCC, scv, mineral, gas, m_CommandQueue));
}

void CMilitaryBaseManager::PushCommand(
	CMilitaryBase::CommandID id,
	CMilitaryBase* pPreferred /* = NULL */,
	int priority /* = 0 */)
{
	const CMilitaryBase::ConstructCommand *pCC = CMilitaryBase::GetCommand(id);
	map<int, CommandList>::iterator it = m_CommandQueue.find(pCC->BuilderID);
	if (it != m_CommandQueue.end()) {
		// Found
		if (priority <= 0) {
			it->second.push_back(CMilitaryBase::CommandObject(id, pPreferred, priority));
			return;
		}
		for (list<CMilitaryBase::CommandObject>::reverse_iterator lit = it->second.rbegin();
			lit != it->second.rend(); ++lit) {
			if (lit->Priority >= priority) {
				// Put here.
				list<CMilitaryBase::CommandObject>::iterator cur = lit.base();
				it->second.insert(cur, CMilitaryBase::CommandObject(id, pPreferred, priority));
				return;
			}
		}
	}
	// No entry
	CommandList cl;
	cl.push_back(CMilitaryBase::CommandObject(id, pPreferred, priority));
	pair<map<int, CommandList>::iterator, bool>
		res = m_CommandQueue.insert(pair<int, CommandList>(int(pCC->BuilderID), cl));
	ASSERT(res.second);
}

void CMilitaryBaseManager::CancelCommand(CMilitaryBase::CommandID id)
{
	ASSERT(CMilitaryBase::AvailableCommand(id));

	map<int, CommandList>::iterator it = m_CommandQueue.find(id);
	if (it != m_CommandQueue.end() && it->second.size() > 0) {
		it->second.pop_back();
	}
}

void CMilitaryBaseManager::NotifyCommand()
{
	for (vector<CMilitaryBase*>::iterator it = m_MilitaryBases.begin();
		it != m_MilitaryBases.end(); ++it) {
		(*it)->NotifyCommand();
	}
	m_CheckCommandsCount = 0;
}

bool CMilitaryBaseManager::TryExpandMilitaryBase()
{
}
