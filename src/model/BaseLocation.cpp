#include "BaseLocation.h"
#include "utils/Macro.h"
#include "utils/Calc.h"
#include "utils/Drawer.h"
#include "model/UnitObject.h"
#include "info/Map.h"
#include "sc2api/sc2_unit.h"

const int NearCBaseLocationTileDistance = 20;

CBaseLocation::CBaseLocation(int id,
	const vector<CUnitObject*>& mineral,
	const vector<CUnitObject*>& vespene,
	CUnitObject* pCC /* = NULL */) :
	m_ID(id),
	m_pDMap(NULL),
	m_pCC(pCC),
	m_Minerals(mineral),
	m_Vespenes(vespene),
	m_CenterOfBase(0.0f, 0.0f),
	m_CenterOfResources(0.0f, 0.0f),
	m_CenterOfMinerals(0.0f, 0.0f),
	m_Left(numeric_limits<float>::max()),
	m_Right(numeric_limits<float>::lowest()),
	m_Top(numeric_limits<float>::lowest()),
	m_Bottom(numeric_limits<float>::max())
{
	float resourceCenterX = 0;
	float resourceCenterY = 0;
	float mineralsCenterX = 0;
	float mineralsCenterY = 0;

	static const float RES_WIDTH = 1;
	static const float RES_HEIGHT = 0.5;

	for (vector<CUnitObject*>::const_iterator it = mineral.begin();
		it != mineral.end(); ++it) {
		const sc2::Unit* pU = (*it)->RawUnit();
		resourceCenterX += pU->pos.x;
		resourceCenterY += pU->pos.y;
		mineralsCenterX += pU->pos.x;
		mineralsCenterY += pU->pos.y;

		m_Left = MIN(m_Left, pU->pos.x - RES_WIDTH);
		m_Right = MAX(m_Right, pU->pos.x + RES_WIDTH);
		m_Top = MAX(m_Top, pU->pos.y + RES_HEIGHT);
		m_Bottom = MIN(m_Bottom, pU->pos.y - RES_HEIGHT);
	}

	for (vector<CUnitObject*>::const_iterator it = vespene.begin();
		it != vespene.end(); ++it) {
		const sc2::Unit* pU = (*it)->RawUnit();
		resourceCenterX += pU->pos.x;
		resourceCenterY += pU->pos.y;

		m_Left = MIN(m_Left, pU->pos.x - RES_WIDTH);
		m_Right = MAX(m_Right, pU->pos.x + RES_WIDTH);
		m_Top = MAX(m_Top, pU->pos.y + RES_HEIGHT);
		m_Bottom = MIN(m_Bottom, pU->pos.y - RES_HEIGHT);
	}

	// calculate the center of the resources
	const size_t numResources = mineral.size() + vespene.size();
	const sc2::Point2D centerMinerals(mineralsCenterX / mineral.size(), mineralsCenterY / mineral.size());
	m_CenterOfMinerals = centerMinerals;
	m_CenterOfResources = sc2::Point2D(
		m_Left + (m_Right - m_Left) * 0.5f, m_Top + (m_Bottom - m_Top) * 0.5f);

	m_pDMap = CMap::Instance().CreateDistanceMap(m_CenterOfResources);
	OUTPUT_DEBUG_TRACE("Distance Map (%d) sorted tiles size: %d\n", m_pDMap->SortedTilePositions.size());

	// Calculate center base
	if (pCC != NULL) {
		m_CenterOfBase.x = pCC->RawUnit()->pos.x;
		m_CenterOfBase.y = pCC->RawUnit()->pos.y;
		return;
	}

}

bool CBaseLocation::ContainsPosition(const sc2::Point2D& pos) const
{
	if (!CMap::Instance().IsValid(pos) || (pos.x == 0 && pos.y == 0)) {
		return false;
	}

	return NSCalc::DistanceSqrt(m_CenterOfBase, pos) < 400 &&
		CMap::Instance().TerrainHeight(m_CenterOfBase) == CMap::Instance().TerrainHeight(pos);
}

int CBaseLocation::GroundDistance(const sc2::Point2D& pos) const
{
	const int dist = m_pDMap->Distance(pos.x, pos.y);
	return dist > 0 ? dist : static_cast<int>(NSCalc::Distance(pos, m_CenterOfResources));
}

#if defined(_DEBUG)
void CBaseLocation::Draw()
{
	CDrawer::Instance().DrawSphere(m_CenterOfResources, 1.0f, sc2::Colors::Yellow);
	CDrawer::Instance().DrawBox(m_Left, m_Top, m_Right, m_Bottom, sc2::Colors::White);

	for (float x = m_Left; x < m_Right; ++x) {
		CDrawer::Instance().DrawLine(x, m_Top, x, m_Bottom, sc2::Color(160, 160, 160));
	}

	for (float y = m_Bottom; y < m_Top; ++y) {
		CDrawer::Instance().DrawLine(m_Left, y, m_Right, y, sc2::Color(160, 160, 160));
	}

	for (const auto & mins : m_Minerals) {
		CDrawer::Instance().DrawSphere(mins->RawUnit()->pos, 1.0f, sc2::Colors::Teal);
	}

	for (const auto & ves : m_Vespenes)
	{
		CDrawer::Instance().DrawSphere(ves->RawUnit()->pos, 1.0f, sc2::Colors::Green);
	}

	if (m_bStartLocation)
	{
		CDrawer::Instance().DrawSphere(m_CenterOfBase, 1.0f, sc2::Colors::Red);
	}

	const float ccWidth = 5;
	const float ccHeight = 4;
	const sc2::Point2D boxtl = m_CenterOfBase - sc2::Point2D(ccWidth/2, -ccHeight/2);
	const sc2::Point2D boxbr = m_CenterOfBase + sc2::Point2D(ccWidth/2, -ccHeight/2);

	CDrawer::Instance().DrawBox(boxtl.x, boxtl.y, boxbr.x, boxbr.y, sc2::Colors::Red);

	const sc2::Point2D posEnd = m_CenterOfResources + 1.2f*(m_CenterOfResources - m_CenterOfBase);
	CDrawer::Instance().DrawLine(CMap::Instance().GetClosestBorderPoint(posEnd, 0), posEnd);
}
#endif
