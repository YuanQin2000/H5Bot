#ifndef __INFO_MAP_H__
#define __INFO_MAP_H__

#include <vector>
#include <queue>
#include "utils/Singleton.h"
#include "utils/Trace.h"
#include "sc2api/sc2_common.h"

class sc2::ObservationInterface;
class sc2::QueryInterface;
class CBaseLocation;

using namespace std;

// DistanceMap record all distance value to all tile on the map.
// Notice: the data is huge, so only used for important position, e.g Base Location.
struct DistanceMap {
	DistanceMap(const sc2::Point2D& pos) : StartTile(pos) {}

	int DistanceMap::Distance(int x, int y) const
	{
		return Dist[x][y];
	}

	sc2::Point2D StartTile;
	vector<vector<int>> Dist;
	vector<sc2::Point2D> SortedTilePositions;
	vector<sc2::Point2D> SortedTilePositionsAir;
};

class CMap : public CSingleton<CMap>
{
public:
	~CMap() {}

	void Initialize(const sc2::ObservationInterface* pObj, sc2::QueryInterface* pQuery);

	int	Width() const { return m_Width; }
	int	Height() const { return m_Height; }

	int SectorNumber(int x, int y) const
	{
		return IsValid(x, y) ? m_SectorNumber[x][y] : 0;
	}

	int SectorNumber(const sc2::Point2D& pos) const
	{
		return SectorNumber(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	const float Altitude(const sc2::Point2D pos) const
	{
		return m_pObservation->TerrainHeight(pos);
	}

	const float Altitude(const float x, const float y) const
	{
		return m_pObservation->TerrainHeight(sc2::Point2D(x, y));
	}

	float TerrainHeight(float x, float y) const
	{
		return IsValid(static_cast<int>(x), static_cast<int>(y)) ?
			m_TerrainHeight[static_cast<int>(x)][static_cast<int>(y)] : -1.0f;
	}

	float TerrainHeight(sc2::Point2D pos) const
	{
		return IsValid(pos) ?
			m_TerrainHeight[static_cast<int>(pos.x)][static_cast<int>(pos.y)] : -1.0f;
	}

	bool IsPowered(const sc2::Point2D& pos) const;
	bool IsExplored(const sc2::Point2D& pos) const;

	bool IsVisible(const sc2::Point2D& pos) const
	{
		return IsValid(pos) ?
			m_pObservation->GetVisibility(pos) == sc2::Visibility::Visible : false;
	}

	bool IsValid(int x, int y) const
	{
		return x >= 0 && y >= 0 && x < m_Width && y < m_Height;
	}

	bool IsValid(const sc2::Point2D & pos) const
	{
		return IsValid(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	bool IsConnected(int x1, int y1, int x2, int y2) const;

	bool IsConnected(const sc2::Point2D& from, const sc2::Point2D & to) const
	{
		return IsConnected(
			static_cast<int>(from.x), static_cast<int>(from.y),
			static_cast<int>(to.x), static_cast<int>(to.y));
	}

	bool IsWalkable(const sc2::Point2D& pos) const
	{
		return IsWalkable(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	bool IsWalkable(int x, int y) const
	{
		return IsValid(x, y) && m_Walkable[x][y];
	}

	bool IsBuildable(const sc2::Point2D& pos) const
	{
		return IsBuildable(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	bool IsBuildable(int x, int y) const
	{
		return IsValid(x, y) && m_Buildable[x][y];
	}

	bool IsDepotBuildableTile(const sc2::Point2D& pos) const
	{
		return IsValid(pos) ?
			m_Ramp[static_cast<int>(pos.x)][static_cast<int>(pos.y)] : false;
	}

	bool CalcBuildable(const sc2::Point2D& point) const;
	bool CalcPathable(const sc2::Point2D& point) const;

	bool CanBuildAtPosition(float x, float y, sc2::AbilityID buildID) const
	{
		return m_pQuery->Placement(buildID, sc2::Point2D(x, y));
	}

	DistanceMap* CreateDistanceMap(const sc2::Point2D& startTile) const;
	DistanceMap* CreateAirDistanceMap(const sc2::Point2D& startTile) const;

	sc2::Point2D CalcWallPositionBunker(
		CBaseLocation* self, CBaseLocation* enemy, vector<CBaseLocation*>& others) const;
	sc2::Point2D CalcWallPositionDepot(
		const CBaseLocation* pSelf, const CBaseLocation* pEnemy) const;
	sc2::Point2D CalcBunkerPosition(
		CBaseLocation* self, CBaseLocation* enemy, vector<CBaseLocation*>& others) const;

	const sc2::Point2D GetClosestBorderPoint(sc2::Point2D& pos, int margin) const;
	const sc2::Point2D GetForbiddenCorner(CBaseLocation* where, const int margin) const;
	const bool HasPocketBase(CBaseLocation* self, vector<CBaseLocation*>& others) const;

protected:
	CMap() : m_pObservation(NULL), m_pQuery(NULL), m_Width(0), m_Height(0), m_MaxZ(0.0f) {}

private:
	void ComputeConnectivity();

	sc2::Point2D GetRampPoint(const CBaseLocation* self, const CBaseLocation* enemy) const;

private:
	const sc2::ObservationInterface* m_pObservation;
	sc2::QueryInterface* m_pQuery;

	int	m_Width;
	int	m_Height;
	float m_MaxZ;

	vector<vector<bool>> m_Walkable;     // whether a tile is buildable (includes static resources)
	vector<vector<bool>> m_Buildable;    // whether a tile is buildable (includes static resources)
	vector<vector<bool>> m_Ramp;         // whether a depot is buildable on a tile (illegal within 3 tiles of static resource)
	vector<vector<int>> m_SectorNumber;	 // connectivity sector number, two tiles are ground connected if they have the same number
	vector<vector<float>> m_TerrainHeight; // height of the map at x+0.5, y+0.5
};


#endif