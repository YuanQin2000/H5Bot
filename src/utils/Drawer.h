#ifndef __UTILS_DRAWER_H__
#define __UTILS_DRAWER_H__

#include "Utils/Singleton.h"

struct sc2::Point2D;
struct sc2::Point3D;
struct sc2::Color;
class sc2::DebugInterface;
class sc2::Unit;

using namespace std;

class CDrawer : public CSingleton<CDrawer>
{
public:
	~CDrawer() {}

	void Initialize(sc2::DebugInterface* pDI);

	void DrawLine(float x1, float y1, float x2, float y2, const sc2::Color color);
	void DrawLine(const sc2::Point2D& min, const sc2::Point2D& max, const sc2::Color color);
	void DrawLine(const sc2::Point3D& min, const sc2::Point3D& max, const sc2::Color color);
	void DrawSquare(float x1, float y1, float x2, float y2, const sc2::Color color);
	void DrawBox(float x1, float y1, float x2, float y2, const sc2::Color color);
	void DrawBox(const sc2::Point2D& min, const sc2::Point2D max, const sc2::Color);
	void DrawSphere(float x1, float x2, float radius, const sc2::Color colore);
	void DrawSphere(const sc2::Point2D& pos, float radius, const sc2::Color colore);
	void DrawText(const sc2::Point2D& pos, const string& str, const sc2::Color colore);
	void DrawTextScreen(const sc2::Point2D& pos, const string& str, const sc2::Color color);
	void DrawBoxAroundUnit(const sc2::Unit* pUnit, sc2::Color color);
	void DrawSphereAroundUnit(const sc2::Unit* pUnit, sc2::Color color);

private:
	CDrawer() : m_pDebugObj(nullptr) {}

private:
	sc2::DebugInterface* m_pDebugObj;
};

#if defined(_DEBUG)

#define DRAW_LINE(a, b, c, d, e) CDrawer::Instance().DrawLine(a, b, c, d, e)
#define DRAW_LINE_POINT2D(a, b, c) CDrawer::Instance().DrawLine(a, b, c)
#define DRAW_LINE_POINT3D(a, b, c) CDrawer::Instance().DrawLine(a, b, c)
#define DRAW_SQUARE(a, b, c, d, e) CDrawer::Instance().DrawSquare(a, b, c, d, e)
#define DRAW_BOX(a, b, c, d, e) CDrawer::Instance().DrawBox(a, b, c, d, e)
#define DRAW_BOX_POINT2D(a, b, c) CDrawer::Instance().DrawBox(a, b, c)
#define DRAW_SPHERE(a, b, c, d) CDrawer::Instance().DrawSphere(a, b, c, d)
#define DRAW_SPHERE_POINT2D(a, b, c) CDrawer::Instance().DrawSphere(a, b, c)
#define DRAW_TEXT(a, b, c) CDrawer::Instance().DrawText(a, b, c)
#define DRAW_TEXT_SCREEN(a, b, c) CDrawer::Instance().DrawTextScreen(a, b, c)
#define DRAW_BOX_AROUND_UNIT(a, b) CDrawer::Instance().DrawBoxAroundUnit(a, b)
#define DRAW_SPHERE_AROUND_UNIT(a, b) CDrawer::Instance().DrawSphereAroundUnit(a, b)

#else

#define DRAW_LINE(a, b, c, d, e)
#define DRAW_LINE_POINT2D(a, b, c)
#define DRAW_LINE_POINT3D(a, b, c)
#define DRAW_SQUARE(a, b, c, d, e)
#define DRAW_BOX(a, b, c, d, e)
#define DRAW_BOX_POINT2D(a, b, c)
#define DRAW_SPHERE(a, b, c, d)
#define DRAW_SPHERE_POINT2D(a, b, c)
#define DRAW_TEXT(a, b, c)
#define DRAW_TEXT_SCREEN(a, b, c)
#define DRAW_BOX_AROUND_UNIT(a, b)
#define DRAW_SPHERE_AROUND_UNIT(a, b)

#endif

#endif