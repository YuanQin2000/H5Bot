#ifndef __MODEL_BASE_LOCATION_H__
#define __MODEL_BASE_LOCATION_H__

#include <vector>
#include "UnitObject.h"
#include "sc2api/sc2_unit.h"

#if defined(_DEBUG)
#define DRAW_BASE_LOCATION(loc) (loc).Draw()
#else
#define DRAW_BASE_LOCATION
#endif

using namespace std;

struct DistanceMap;

class CBaseLocation
{
public:
	CBaseLocation(int id,
		const vector<CUnitObject*>& mineral,
		const vector<CUnitObject*>& vespene,
		CUnitObject* pCC = NULL);

	bool ContainsPosition(const sc2::Point2D & pos) const;
	int GroundDistance(const sc2::Point2D & pos) const;

	const int getBaseID() const
	{
		return m_ID;
	}

	const sc2::Point2D& CenterOfRessources() const
	{
		return m_CenterOfResources;
	}

	const sc2::Point2D& CenterOfMinerals() const
	{
		return m_CenterOfMinerals;
	}

	const sc2::Point2D& CenterOfBase() const
	{
		return m_CenterOfBase;
	}

	const vector<CUnitObject*>& GetVespenes() const
	{
		return m_Vespenes;
	}

	const vector<CUnitObject*>& GetMinerals() const
	{
		return m_Minerals;
	}

	bool InResourceBox(int x, int y) const
	{
		return x >= m_Left && x < m_Right && y < m_Top && y >= m_Bottom;
	}

	bool CBaseLocation::IsStartLocation() const
	{
		return m_bStartLocation;
	}

	const vector<sc2::Point2D>& getClosestTiles() const
	{
		return m_pDMap->SortedTilePositions;
	}

	const CUnitObject* GetTownHall() const
	{
		return m_pCC;
	}

#if defined(_DEBUG)
	void Draw();
#endif

private:
	int m_ID;
	DistanceMap* m_pDMap;	// Owned
	CUnitObject* m_pCC;
	vector<CUnitObject*> m_Minerals;
	vector<CUnitObject*> m_Vespenes;

	sc2::Point2D m_CenterOfBase;
	sc2::Point2D m_CenterOfResources;
	sc2::Point2D m_CenterOfMinerals;

	bool m_bStartLocation;

	// Rectangle scope
	float m_Left;
	float m_Right;
	float m_Top;
	float m_Bottom;
};

#endif