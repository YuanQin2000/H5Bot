#include "Map.h"
#include "utils/Calc.h"
#include "utils/Trace.h"
#include "utils/Helper.h"
#include "utils/Macro.h"
#include "model/BaseLocation.h"
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_unit.h"
#include "sc2api/sc2_common.h"

using namespace std;

static const size_t LegalActions = 4;
static const int actionX[LegalActions] = { 1, -1, 0, 0 };
static const int actionY[LegalActions] = { 0, 0, 1, -1 };

void CMap::Initialize(
	const sc2::ObservationInterface* pObj, sc2::QueryInterface* pQuery)
{
	ASSERT(m_pObservation == NULL);
	ASSERT(m_pQuery == NULL);
	ASSERT(pObj);
	ASSERT(pQuery);

	const sc2::GameInfo& info(pObj->GetGameInfo());

	m_pObservation = pObj;
	m_pQuery = pQuery;

	m_Width = info.width;
	m_Height = info.height;

	m_Walkable = vector<vector<bool>>(m_Width, vector<bool>(m_Height, true));
	m_Buildable = vector<vector<bool>>(m_Width, vector<bool>(m_Height, false));
	m_Ramp = vector<vector<bool>>(m_Width, vector<bool>(m_Height, false));
	m_SectorNumber = vector<vector<int>>(m_Width, vector<int>(m_Height, 0));
	m_TerrainHeight = vector<vector<float>>(m_Width, vector<float>(m_Height, 0.0f));

	for (int x = 0; x < m_Width; ++x) {
		for (int y = 0; y < m_Height; ++y) {
			m_Buildable[x][y] = CalcBuildable(sc2::Point2D(x + 0.5f, y + 0.5f));
			m_Walkable[x][y] = m_Buildable[x][y] || CalcPathable(sc2::Point2D(x + 0.5f, y + 0.5f));
			m_TerrainHeight[x][y] = pObj->TerrainHeight(sc2::Point2D(x + 0.5f, y + 0.5f));
			m_Ramp[x][y] = m_Walkable[x][y] || !m_Buildable[x][y];
		}
	}

	for (const auto & unit : pObj->GetUnits(sc2::Unit::Alliance::Neutral)) {
		m_MaxZ = unit->pos.z > m_MaxZ ? unit->pos.z : m_MaxZ;
	}

	sc2::Units minerals = pObj->GetUnits(sc2::Unit::Alliance::Neutral, sc2::IsUnits(NSHelper::GetMineralTypes()));
	for (const auto & mineral : minerals) {
		for (int i = -1; i <= 1; ++i) {
			for (int j = -1; j <= 1; ++j) {
				m_Walkable[static_cast<int>(mineral->pos.x) + i][static_cast<int>(mineral->pos.y) + j] = false;
			}
		}
	}
	ComputeConnectivity();
}

void CMap::ComputeConnectivity()
{
	// the fringe data structe we will use to do our BFS searches
	vector<sc2::Point2D> fringe;
	fringe.reserve(static_cast<size_t>(m_Width) * static_cast<size_t>(m_Height));
	int sectorNumber = 0;

	// for every tile on the map, do a connected flood fill using BFS
	for (int x = 0; x < m_Width; ++x) {
		for (int y = 0; y < m_Height; ++y) {
			// if the sector is not currently 0, or the map isn't walkable here, then we can skip this tile
			if (SectorNumber(x, y) != 0 || !IsWalkable(x, y)) {
				continue;
			}

			// increase the sector number, so that walkable tiles have sectors 1-N
			sectorNumber++;

			// reset the fringe for the search and add the start tile to it
			fringe.clear();
			fringe.push_back(sc2::Point2D(x + 0.5f, y + 0.5f));
			m_SectorNumber[x][y] = sectorNumber;

			// do the BFS, stopping when we reach the last element of the fringe
			for (size_t fringeIndex = 0; fringeIndex < fringe.size(); ++fringeIndex) {
				auto & tile = fringe[fringeIndex];

				// check every possible child of this tile
				for (size_t a = 0; a < LegalActions; ++a) {
					sc2::Point2D nextTile(tile.x + actionX[a], tile.y + actionY[a]);

					// if the new tile is inside the map bounds, is walkable, and has not been assigned a sector, add it to the current sector and the fringe
					if (IsValid(nextTile) && IsWalkable(nextTile) && (SectorNumber(nextTile) == 0)) {
						m_SectorNumber[static_cast<int>(nextTile.x)][static_cast<int>(nextTile.y)] = sectorNumber;
						fringe.push_back(nextTile);
					}
				}
			}
		}
	}
}

bool CMap::IsConnected(int x1, int y1, int x2, int y2) const
{
	if (IsValid(x1, y1) && IsValid(x2, y2)) {
		int s1 = SectorNumber(x1, y1);
		int s2 = SectorNumber(x2, y2);

		return s1 != 0 && (s1 == s2);
	}
	return false;
}

bool CMap::IsExplored(const sc2::Point2D& pos) const
{
	if (!IsValid(pos)) {
		return false;
	}
	sc2::Visibility vis = m_pObservation->GetVisibility(pos);
	return vis == sc2::Visibility::Fogged || vis == sc2::Visibility::Visible;
}

bool CMap::IsPowered(const sc2::Point2D& pos) const
{
	for (const auto & powerSource : m_pObservation->GetPowerSources()) {
		if (NSCalc::Distance(pos, powerSource.position) < powerSource.radius) {
			return true;
		}
	}
	return false;
}

bool CMap::CalcBuildable(const sc2::Point2D & point) const
{
	const sc2::GameInfo& info(m_pObservation->GetGameInfo());
	sc2::Point2DI pointI((int)point.x, (int)point.y);
	if (pointI.x < 0 || pointI.x >= info.width || pointI.y < 0 || pointI.y >= info.width) {
		return false;
	}
	unsigned char encodedPlacement = info.placement_grid.data[pointI.x + ((info.height - 1) - pointI.y) * info.width];
	return encodedPlacement == 255 ? true : false;
}

bool CMap::CalcPathable(const sc2::Point2D& point) const
{
	const sc2::GameInfo& info(m_pObservation->GetGameInfo());

	sc2::Point2DI pointI((int)point.x, (int)point.y);
	if (pointI.x < 0
		|| pointI.x >= info.width
		|| pointI.y < 0
		|| pointI.y >= info.width) {
		return false;
	}

	unsigned char encodedPlacement =
		info.pathing_grid.data[pointI.x + ((info.height - 1) - pointI.y) * info.width];
	return encodedPlacement == 255 ? false : true;
}

sc2::Point2D CMap::GetRampPoint(const CBaseLocation* self, const CBaseLocation* enemy) const
{
	const sc2::Point2D startPoint = self->CenterOfBase()
		+ NSCalc::NormalizeVector(self->CenterOfBase() - self->CenterOfMinerals(), 5.0f);
	const float startHeight = m_pObservation->TerrainHeight(startPoint);

	sc2::Point2D currentPos = sc2::Point2D(round(startPoint.x) + 0.5f, round(startPoint.y) + 0.5f);
	const sc2::Point2D enemyPoint = m_pObservation->GetGameInfo().enemy_start_locations.front();
	const float stepSize = 1.0;
	const sc2::Point2D xMove(stepSize, 0.0f);
	const sc2::Point2D yMove(0.0f, stepSize);
	int currentWalkingDistance = enemy->GroundDistance(startPoint);
	bool foundNewPos = true;
	while (foundNewPos) {
		foundNewPos = false;
		for (float i = -1.0f; i <= 1.0f; ++i) {
			for (float j = -1.0f; j <= 1.0f; ++j) {
				if (i != 0.0f || j != 0.0f) {
					const sc2::Point2D newPos = currentPos + i * xMove + j * yMove;
					const int dist = enemy->GroundDistance(newPos);
					if (m_pObservation->TerrainHeight(newPos) == startHeight
						&& dist > 0 && m_pObservation->IsPathable(newPos)
						&& (m_pObservation->IsPlacable(currentPos) || m_pObservation->IsPlacable(newPos))) {
						if ((m_pObservation->IsPlacable(newPos + sc2::Point2D(0.0f, 1.0f)) || m_pObservation->IsPlacable(newPos - sc2::Point2D(0.0f, 1.0f)))
							&& (m_pObservation->IsPlacable(newPos + sc2::Point2D(1.0f, 0.0f)) || m_pObservation->IsPlacable(newPos - sc2::Point2D(1.0f, 0.0f)))) {
							bool newPosBetter = false;
							if (currentWalkingDistance > dist) {
								newPosBetter = true;
							} else if (currentWalkingDistance == dist && m_pObservation->IsPlacable(currentPos)) {
								if (!m_pObservation->IsPlacable(newPos)) {
									newPosBetter = true;
								} else {
									const vector<float> dists = m_pQuery->PathingDistance({
											{ sc2::NullTag, currentPos, enemy->CenterOfMinerals() },
											{ sc2::NullTag, newPos, enemy->CenterOfMinerals() }
										});
									if (dists.front() > dists.back()) {
										newPosBetter = true;
									}
								}
							}

							if (newPosBetter) {
								currentWalkingDistance = dist;
								currentPos = newPos;
								foundNewPos = true;
								break;
							}
						}
					}
				}
			}
			if (foundNewPos) {
				break;
			}
		}
	}

	if (NSCalc::Distance(startPoint, currentPos) < 20.0f) {
		return currentPos;
	}

	return sc2::Point2D(0.0f, 0.0f);
}

sc2::Point2D CMap::CalcWallPositionBunker(
	CBaseLocation* self, CBaseLocation* enemy, vector<CBaseLocation*>& others) const
{
	if (HasPocketBase(self, others)) {
		return sc2::Point2D{ 0.0f, 0.0f };
	}
	sc2::Point2D rampPoint = GetRampPoint(self, enemy);
	if (rampPoint == sc2::Point2D{ 0.0f, 0.0f }) {
		return rampPoint;
	}
	int rampType = 0;
	if (!m_pObservation->IsPlacable(rampPoint + sc2::Point2D{ 0, 1 })) { // North 
		rampType += 10;
	}
	if (!m_pObservation->IsPlacable(rampPoint + sc2::Point2D{ 1, 0 })) { // East
		rampType += 1;
	}
	sc2::Point2D bunkerPosition = sc2::Point2D{ 0.0f, 0.0f };
	switch (rampType) {
	case(0):  // SW
		{
			while (!m_pObservation->IsPlacable(rampPoint + sc2::Point2D{ 1.0f, -1.0f })) {
				rampPoint += sc2::Point2D{ 1.0f, -1.0f };
			}
			int rampLength = 1;
			while (!m_pObservation->IsPlacable(rampPoint - static_cast<float>(rampLength)*sc2::Point2D{ 1.0f, -1.0f })) {
				++rampLength;
			}
			if (rampLength == 6) {
				bunkerPosition = rampPoint + sc2::Point2D(-1.0f, 4.0f);
			}
			break;
		}
	case(1):  // SE
		{
			while (!m_pObservation->IsPlacable(rampPoint + sc2::Point2D{ 1.0f, 1.0f })) {
				rampPoint += sc2::Point2D{ 1.0f, 1.0f };
			}
			int rampLength = 1;
			while (!m_pObservation->IsPlacable(rampPoint - static_cast<float>(rampLength)*sc2::Point2D{ 1.0f, 1.0f })) {
				++rampLength;
			}
			if (rampLength == 4) {
				bunkerPosition = rampPoint + sc2::Point2D(-4.0f, 1.0f);
			} else if (rampLength == 6) {
				bunkerPosition = rampPoint + sc2::Point2D(-4.0f, -1.0f);
			}
			break;
		}
	case(10):  // NW
		{
			while (!m_pObservation->IsPlacable(rampPoint + sc2::Point2D{ -1.0f, -1.0f })) {
				rampPoint += sc2::Point2D{ -1.0f, -1.0f };
			}
			int rampLength = 1;
			while (!m_pObservation->IsPlacable(rampPoint - static_cast<float>(rampLength)*sc2::Point2D{ -1.0f, -1.0f })) {
				++rampLength;
			}
			if (rampLength == 4) {
				bunkerPosition = rampPoint + sc2::Point2D(4.0f, -1.0f);
			} else if (rampLength == 6) {
				bunkerPosition = rampPoint + sc2::Point2D(4.0f, 1.0f);
			}
			break;
		}
	case(11):  // NE
	{
		while (!m_pObservation->IsPlacable(rampPoint + sc2::Point2D{ -1.0f, 1.0f })) {
			rampPoint += sc2::Point2D{ -1.0f, 1.0f };
		}
		int rampLength = 1;
		while (!m_pObservation->IsPlacable(rampPoint - static_cast<float>(rampLength)*sc2::Point2D{ -1.0f, 1.0f })) {
			++rampLength;
		}
		if (rampLength == 6) {
			bunkerPosition = rampPoint + sc2::Point2D(1.0f, -4.0f);
		}
		break;
	}
	}
	return bunkerPosition;
}

sc2::Point2D CMap::CalcWallPositionDepot(const CBaseLocation* pSelf, const CBaseLocation* pEnemy) const
{
	if (!pSelf || !pSelf->GetTownHall() || !pSelf->GetTownHall()->RawUnit()->is_alive) {
		return sc2::Point2D{ 0.0f, 0.0f };
	}
	sc2::Point2D rampPoint = GetRampPoint(pSelf, pEnemy);
	if (rampPoint == sc2::Point2D{ 0.0f, 0.0f }) {
		return rampPoint;
	}
	int rampType = 0;
	if (!m_pObservation->IsPlacable(rampPoint + sc2::Point2D{ 0, 1 })) { // North
		rampType += 10;
	}
	if (!m_pObservation->IsPlacable(rampPoint + sc2::Point2D{ 1, 0 })) { // East
		rampType += 1;
	}

	vector<sc2::Point2D> positions;
	switch (rampType) {
	case(0):  // SW
		{
			while (!m_pObservation->IsPlacable(rampPoint + sc2::Point2D{ 1.0f, -1.0f })) {
				rampPoint += sc2::Point2D{ 1.0f, -1.0f };
			}
			int rampLength = 1;
			while (!m_pObservation->IsPlacable(rampPoint - static_cast<float>(rampLength)*sc2::Point2D{ 1.0f, -1.0f })) {
				++rampLength;
			}
			if (rampLength == 2) {
				positions = { rampPoint + sc2::Point2D(0.5f, 1.5f), rampPoint + sc2::Point2D(1.5f, -0.5f), rampPoint + sc2::Point2D(-1.5f, 2.5f) };
			} else if (rampLength == 6) {
				positions = { rampPoint + sc2::Point2D(0.5f, 1.5f), rampPoint + sc2::Point2D(1.5f, -0.5f), rampPoint + sc2::Point2D(-3.5f, 5.5f), rampPoint + sc2::Point2D(-5.5f, 6.5f) };
			}
			break;
		}
	case(1):  // SE
		{
			while (!m_pObservation->IsPlacable(rampPoint + sc2::Point2D{ 1.0f, 1.0f })) {
				rampPoint += sc2::Point2D{ 1.0f, 1.0f };
			}
			int rampLength = 1;
			while (!m_pObservation->IsPlacable(rampPoint - static_cast<float>(rampLength)*sc2::Point2D{ 1.0f, 1.0f })) {
				++rampLength;
			}
			if (rampLength == 2) {
				positions = { rampPoint + sc2::Point2D(0.5f, 1.5f), rampPoint + sc2::Point2D(-1.5f, 0.5f), rampPoint + sc2::Point2D(-2.5f, -1.5f) };
			} else if (rampLength == 4) {
				positions = { rampPoint + sc2::Point2D(0.5f, 1.5f), rampPoint + sc2::Point2D(-1.5f, 0.5f), rampPoint + sc2::Point2D(-3.5f, -1.5f), rampPoint + sc2::Point2D(-4.5f, -3.5f) };
			} else if (rampLength == 6) {
				positions = { rampPoint + sc2::Point2D(0.5f, 1.5f), rampPoint + sc2::Point2D(-1.5f, 0.5f), rampPoint + sc2::Point2D(-5.5f, -3.5f), rampPoint + sc2::Point2D(-6.5f, -5.5f) };
			}
			break;
		}
	case(10):  // NW
		{
			while (!m_pObservation->IsPlacable(rampPoint + sc2::Point2D{ -1.0f, -1.0f })) {
				rampPoint += sc2::Point2D{ -1.0f, -1.0f };
			}
			int rampLength = 1;
			while (!m_pObservation->IsPlacable(rampPoint - static_cast<float>(rampLength)*sc2::Point2D{ -1.0f, -1.0f })) {
				++rampLength;
			}
			if (rampLength == 2) {
				positions = { rampPoint + sc2::Point2D(-0.5f, -1.5f), rampPoint + sc2::Point2D(1.5f, -0.5f), rampPoint + sc2::Point2D(2.5f, 1.5f) };
			} else if (rampLength == 4) {
				positions = { rampPoint + sc2::Point2D(-0.5f, -1.5f), rampPoint + sc2::Point2D(1.5f, -0.5f), rampPoint + sc2::Point2D(3.5f, 1.5f), rampPoint + sc2::Point2D(4.5f, 3.5f) };
			} else if (rampLength == 6) {
				positions = { rampPoint + sc2::Point2D(-0.5f, -1.5f), rampPoint + sc2::Point2D(1.5f, -0.5f), rampPoint + sc2::Point2D(5.5f, 3.5f), rampPoint + sc2::Point2D(6.5f, 5.5f) };
			}
			break;
		}
	case(11):  // NE
		{
			while (!m_pObservation->IsPlacable(rampPoint + sc2::Point2D{ -1.0f, 1.0f })) {
				rampPoint += sc2::Point2D{ -1.0f, 1.0f };
			}
			int rampLength = 1;
			while (!m_pObservation->IsPlacable(rampPoint - static_cast<float>(rampLength)*sc2::Point2D{ -1.0f, 1.0f })) {
				++rampLength;
			}
			if (rampLength == 2) {
				positions = { rampPoint + sc2::Point2D(-0.5f, -1.5f), rampPoint + sc2::Point2D(-1.5f, 0.5f), rampPoint + sc2::Point2D(1.5f, -2.5f) };
			}
			if (rampLength == 6) {
				positions = { rampPoint + sc2::Point2D(-0.5f, -1.5f), rampPoint + sc2::Point2D(-1.5f, 0.5f), rampPoint + sc2::Point2D(3.5f, -5.5f), rampPoint + sc2::Point2D(5.5f, -6.5f) };
			}
			break;
		}
	}

	const sc2::ABILITY_ID depotID = sc2::ABILITY_ID::BUILD_SUPPLYDEPOT;
	std::vector<sc2::QueryInterface::PlacementQuery> placementBatched;
	for (const auto & pos : positions) {
		placementBatched.push_back({ depotID, pos });
	}
	std::vector<bool> result = m_pQuery->Placement(placementBatched);
	for (int i = 0; i < result.size(); ++i) {
		if (result[i]) {
			return positions[i];
		}
	}
	return sc2::Point2D{ 0.0f, 0.0f };
}

sc2::Point2D CMap::CalcBunkerPosition(
	CBaseLocation* self, CBaseLocation* enemy, vector<CBaseLocation*>& others) const
{
	if (HasPocketBase(self, others)) {
		const sc2::Point2D startPoint(self->CenterOfBase());
		const float startHeight = m_pObservation->TerrainHeight(startPoint);
		sc2::Point2D currentPos = startPoint;
		const float stepSize = 2.0;
		const sc2::Point2D xMove(stepSize, 0.0f);
		const sc2::Point2D yMove(0.0f, stepSize);
		int currentWalkingDistance = enemy->GroundDistance(startPoint);
		bool foundNewPos = true;
		while (foundNewPos) {
			foundNewPos = false;
			for (float i = -1.0f; i <= 1.0f; ++i) {
				for (float j = -1.0f; j <= 1.0f; ++j) {
					if (i != 0.0f || j != 0.0f) {
						const sc2::Point2D newPos = currentPos + i * xMove + j * yMove;
						const int dist = enemy->GroundDistance(newPos);
						if (m_pObservation->TerrainHeight(newPos) == startHeight
							&& dist > 0 && currentWalkingDistance > dist) {
							currentWalkingDistance = dist;
							currentPos = newPos;
							foundNewPos = true;
							break;
						}
					}
				}
				if (foundNewPos) {
					break;
				}
			}
		}
		return currentPos;
	}

	return CalcWallPositionBunker(self, enemy);
}

const sc2::Point2D CMap::GetForbiddenCorner(CBaseLocation* where, const int margin) const
{
	if (where == NULL) {
		return sc2::Point2D(-1.0f, -1.0f);
	}

	const sc2::Point2D pos = where->CenterOfBase();

	const float x_min = static_cast<float>(m_pObservation->GetGameInfo().playable_min.x + margin);
	const float x_max = static_cast<float>(m_pObservation->GetGameInfo().playable_max.x - margin);
	const float y_min = static_cast<float>(m_pObservation->GetGameInfo().playable_min.y + margin);
	const float y_max = static_cast<float>(m_pObservation->GetGameInfo().playable_max.y - margin);
	if (pos.x - x_min < x_max - pos.x) {
		if (pos.y - y_min < y_max - pos.y) {
			return sc2::Point2D(x_min, y_min);
		} else {
			return sc2::Point2D(x_min, y_max);
		}
	} else {
		if (pos.y - y_min < y_max - pos.y) {
			return sc2::Point2D(x_max, y_min);
		} else {
			return sc2::Point2D(x_max, y_max);
		}
	}
}

const sc2::Point2D CMap::GetClosestBorderPoint(sc2::Point2D& pos, int margin) const
{
	const float x_min = static_cast<float>(m_pObservation->GetGameInfo().playable_min.x + margin);
	const float x_max = static_cast<float>(m_pObservation->GetGameInfo().playable_max.x - margin);
	const float y_min = static_cast<float>(m_pObservation->GetGameInfo().playable_min.y + margin);
	const float y_max = static_cast<float>(m_pObservation->GetGameInfo().playable_max.y - margin);
	if (pos.x - x_min < x_max - pos.x) {
		if (pos.y - y_min < y_max - pos.y) {
			if (pos.x - x_min < pos.y - y_min) {
				return sc2::Point2D(x_min, pos.y);
			} else {
				return sc2::Point2D(pos.x, y_min);
			}
		} else {
			if (pos.x - x_min < y_max - pos.y) {
				return sc2::Point2D(x_min, pos.y);
			} else {
				return sc2::Point2D(pos.x, y_max);
			}
		}
	} else {
		if (pos.y - y_min < y_max - pos.y) {
			if (x_max - pos.x < pos.y - y_min) {
				return sc2::Point2D(x_max, pos.y);
			} else {
				return sc2::Point2D(pos.x, y_min);
			}
		} else {
			if (x_max - pos.x < y_max - pos.y) {
				return sc2::Point2D(x_max, pos.y);
			} else {
				return sc2::Point2D(pos.x, y_max);
			}
		}
	}
}

const bool CMap::HasPocketBase(CBaseLocation* self, vector<CBaseLocation*>& others) const
{
	const CBaseLocation * firstExe = nullptr;
	int minDistance = numeric_limits<int>::max();
	for (const auto & base : others) {
		auto tile = base->CenterOfBase();
		int distanceFromHome = self->GroundDistance(tile);
		if (distanceFromHome <= 0) {
			continue;
		}
		if (!firstExe || distanceFromHome < minDistance) {
			firstExe = base;
			minDistance = distanceFromHome;
		}
	}

	const sc2::Point2D enemyStartBase = m_pObservation->GetGameInfo().enemy_start_locations.front();
	return firstExe ? self->GroundDistance(enemyStartBase) <= firstExe->GroundDistance(enemyStartBase) : false;
}

// Computes m_dist[x][y] = ground distance from (startX, startY) to (x,y)
// Uses BFS, since the map is quite large and DFS may cause a stack overflow
DistanceMap* CMap::CreateDistanceMap(const sc2::Point2D& startTile) const
{
	DistanceMap* pDMap = new DistanceMap(startTile);
	pDMap->Dist = vector<vector<int>>(m_Width, vector<int>(m_Height, -1));
	pDMap->SortedTilePositions.reserve(size_t(m_Width) * size_t(m_Height));

	// the fringe for the BFS we will perform to calculate distances
	vector<sc2::Point2D> fringe;
	fringe.reserve(static_cast<size_t>(m_Width) * static_cast<size_t>(m_Height));
	fringe.push_back(startTile);
	pDMap->SortedTilePositions.push_back(startTile);

	pDMap->Dist[(int)startTile.x][(int)startTile.y] = 0;

	for (size_t fringeIndex = 0; fringeIndex < fringe.size(); ++fringeIndex) {
		const sc2::Point2D & tile = fringe[fringeIndex];

		// check every possible child of this tile
		for (size_t a = 0; a < LegalActions; ++a) {
			sc2::Point2D nextTile(tile.x + actionX[a], tile.y + actionY[a]);

			// if the new tile is inside the map bounds, is walkable,
			// and has not been visited yet, set the distance of its parent + 1
			if (IsWalkable(nextTile) &&
				pDMap->Dist[int(nextTile.x)][int(nextTile.y)] == -1) {
				pDMap->Dist[(int)nextTile.x][(int)nextTile.y] = pDMap->Dist[(int)tile.x][(int)tile.y] + 1;
				fringe.push_back(nextTile);
				pDMap->SortedTilePositions.push_back(nextTile);
			}
		}
	}
	return pDMap;
}

DistanceMap* CMap::CreateAirDistanceMap(const sc2::Point2D& startTile) const
{
	DistanceMap* pDMap = new DistanceMap(startTile);

	pDMap->Dist = vector<vector<int>>(m_Width, vector<int>(m_Height, -1));
	pDMap->SortedTilePositionsAir.reserve(size_t(m_Width) * size_t(m_Height));

	// the fringe for the BFS we will perform to calculate distances
	vector<sc2::Point2D> fringe;
	fringe.reserve(static_cast<size_t>(m_Width) * static_cast<size_t>(m_Height));
	fringe.push_back(startTile);
	pDMap->SortedTilePositionsAir.push_back(startTile);

	pDMap->Dist[(int)startTile.x][(int)startTile.y] = 0;

	for (size_t fringeIndex = 0; fringeIndex < fringe.size(); ++fringeIndex) {
		const sc2::Point2D & tile = fringe[fringeIndex];

		// check every possible child of this tile
		for (size_t a = 0; a < LegalActions; ++a) {
			sc2::Point2D nextTile(tile.x + actionX[a], tile.y + actionY[a]);

			// if the new tile is inside the map bounds, is walkable,
			// and has not been visited yet, set the distance of its parent + 1
			if (IsValid(nextTile) &&
				pDMap->Dist[int(nextTile.x)][int(nextTile.y)] == -1) {
				pDMap->Dist[(int)nextTile.x][(int)nextTile.y] = pDMap->Dist[(int)tile.x][(int)tile.y] + 1;
				fringe.push_back(nextTile);
				pDMap->SortedTilePositionsAir.push_back(nextTile);
			}
		}
	}
}
