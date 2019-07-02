#include "Drawer.h"
#include "info/Map.h"
#include "utils/Trace.h"
#include "utils/Macro.h"
#include "sc2api/sc2_client.h"
#include "sc2api/sc2_common.h"
#include "sc2api/sc2_unit.h"
#include "sc2api/sc2_interfaces.h"

void CDrawer::Initialize(sc2::DebugInterface* pDI)
{
	ASSERT(m_pDebugObj == NULL);
	ASSERT(pDI);
	m_pDebugObj = pDI;
}

void CDrawer::DrawLine(float x1, float y1, float x2, float y2, const sc2::Color color)
{
	const float maxZ = CMap::Instance().Altitude(x1, y1);
	m_pDebugObj->DebugLineOut(sc2::Point3D(x1, y1, maxZ + 0.2f), sc2::Point3D(x2, y2, maxZ + 0.2f), color);
}

void CDrawer::DrawLine(const sc2::Point2D& min, const sc2::Point2D& max, const sc2::Color color)
{
	const float maxZ = MAX(CMap::Instance().Altitude(min), CMap::Instance().Altitude(max));
	m_pDebugObj->DebugLineOut(sc2::Point3D(min.x, min.y, maxZ + 0.2f), sc2::Point3D(max.x, max.y, maxZ + 0.2f), color);
}

void CDrawer::DrawLine(const sc2::Point3D& min, const sc2::Point3D& max, const sc2::Color color)
{
	m_pDebugObj->DebugLineOut(min + sc2::Point3D(0.0f, 0.0f, 0.2f), max + sc2::Point3D(0.0f, 0.0f, 0.2f), color);
}

void CDrawer::DrawSquare(float x1, float y1, float x2, float y2, const sc2::Color color)
{
	const float maxZ = CMap::Instance().Altitude(x1, y1) + 0.1f;
	m_pDebugObj->DebugLineOut(sc2::Point3D(x1, y1, maxZ), sc2::Point3D(x1, y2, maxZ), color);
	m_pDebugObj->DebugLineOut(sc2::Point3D(x1, y2, maxZ), sc2::Point3D(x2, y2, maxZ), color);
	m_pDebugObj->DebugLineOut(sc2::Point3D(x2, y2, maxZ), sc2::Point3D(x2, y1, maxZ), color);
	m_pDebugObj->DebugLineOut(sc2::Point3D(x2, y1, maxZ), sc2::Point3D(x1, y1, maxZ), color);
}

void CDrawer::DrawBox(float x1, float y1, float x2, float y2, const sc2::Color color)
{
	m_pDebugObj->DebugBoxOut(
		sc2::Point3D(x1, y1, 2.0f + CMap::Instance().Altitude(x1, y1)),
		sc2::Point3D(x2, y2, 5.0f - CMap::Instance().Altitude(x1, y1)), color);
}

void CDrawer::DrawBox(const sc2::Point2D& min, const sc2::Point2D& max, const sc2::Color color)
{
	m_pDebugObj->DebugBoxOut(
		sc2::Point3D(min.x, min.y, CMap::Instance().Altitude(min) + 2.0f),
		sc2::Point3D(max.x, max.y, CMap::Instance().Altitude(min) - 5.0f), color);
}

void CDrawer::DrawSphere(float x1, float x2, float radius, const sc2::Color color)
{
	m_pDebugObj->DebugSphereOut(sc2::Point3D(pos.x, pos.y, CMap::Instance().Altitude(pos) + 0.1f), radius, color);
}

void CDrawer::DrawSphere(const sc2::Point2D& pos, float radius, const sc2::Color color)
{
	m_pDebugObj->DebugSphereOut(sc2::Point3D(x, y, CMap::Instance().Altitude(x, y)), radius, color);
}

void CDrawer::DrawText(const sc2::Point2D& pos, const string& str, const sc2::Color color)
{
	m_pDebugObj->DebugTextOut(str, sc2::Point3D(pos.x, pos.y, CMap::Instance().Altitude(pos)), color);
}

void CDrawer::DrawTextScreen(const sc2::Point2D& pos, const string& str, const sc2::Color color)
{
	m_pDebugObj->DebugTextOut(str, pos, color, 8U);
}

void CDrawer::DrawBoxAroundUnit(const sc2::Unit* pUnit, sc2::Color color)
{
	DrawSquare(pUnit->pos.x - pUnit->radius,
		pUnit->pos.y - pUnit->radius,
		pUnit->pos.x + pUnit->radius,
		pUnit->pos.y + pUnit->radius, color);
}

void CDrawer::DrawSphereAroundUnit(const sc2::Unit* pUnit, sc2::Color color)
{
	DrawSphere(pUnit->pos, pUnit->radius, color);
}