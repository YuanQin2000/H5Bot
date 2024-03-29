#include "CBaseLocationManager.h"
#include "Util.h"
#include "sc2lib/sc2_search.h"
#include "Drawing.h"
#include "CCBot.h"

CBaseLocationManager::CBaseLocationManager(CCBot & bot)
	: m_bot(bot)
{
	
}

void CBaseLocationManager::onStart()
{
	//sc2::search::CalculateExpansionLocations(m_bot.Observation(),m_bot.Query())

	m_tileCBaseLocations = vector<vector<CBaseLocation *>>(m_bot.Map().width(), vector<CBaseLocation *>(m_bot.Map().height(), nullptr));
	m_playerStartingCBaseLocations[Players::Self]  = nullptr;
	m_playerStartingCBaseLocations[Players::Enemy] = nullptr; 
	
	// a CBaseLocation will be anything where there are minerals to mine
	// so we will first look over all minerals and cluster them based on some distance
	const int clusterDistance = 14;
	
	// stores each cluster of resources based on some ground distance
	vector<CUnits> resourceClusters;
	for (const auto & mineral : m_bot.UnitInfo().getUnits(Players::Neutral,Util::getMineralTypes()))
	{
		bool foundCluster = false;
		for (auto & cluster : resourceClusters)
		{
			float dist = Util::Dist(mineral->getPos(), Util::CalcCenter(cluster));
			
			// quick initial air distance check to eliminate most resources
			if (dist < clusterDistance)
			{
				// now do a more expensive ground distance check
				float groundDist = dist; //m_bot.Map().getGroundDistance(mineral.pos, Util::CalcCenter(cluster));
				if (groundDist >= 0 && groundDist < clusterDistance)
				{
					cluster.push_back(mineral);
					foundCluster = true;
					break;
				}
			}
		}

		if (!foundCluster)
		{
			resourceClusters.push_back(vector<CUnit_ptr>());
			resourceClusters.back().push_back(mineral);
		}
	}

	// add geysers only to existing resource clusters
	for (const auto & geyser : m_bot.UnitInfo().getUnits(Players::Neutral, Util::getGeyserTypes()))
	{
		for (auto & cluster : resourceClusters)
		{
			//int groundDist = m_bot.Map().getGroundDistance(geyser.pos, Util::CalcCenter(cluster));
			float groundDist = Util::Dist(geyser->getPos(), Util::CalcCenter(cluster));

			if (groundDist >= 0 && groundDist < clusterDistance)
			{
				cluster.push_back(geyser);
				break;
			}
		}
	}

	// add the base locations if there are more than 4 resouces in the cluster
	int baseID = 0;
	for (const auto & cluster : resourceClusters)
	{
		if (cluster.size() > 4)
		{
			m_CBaseLocationData.push_back(CBaseLocation(m_bot, baseID++, cluster));
		}
	}

	// construct the vectors of base location pointers, this is safe since they will never change
	for (const auto & CBaseLocation : m_CBaseLocationData)
	{
		m_CBaseLocationPtrs.push_back(&CBaseLocation);

		// if it's a start location, add it to the start locations
		if (CBaseLocation.isStartLocation())
		{
			m_startingCBaseLocations.push_back(&CBaseLocation);
		}

		// if it's our starting location, set the pointer
		if (CBaseLocation.isPlayerStartLocation(Players::Self))
		{
			m_playerStartingCBaseLocations[Players::Self] = &CBaseLocation;
		}

		if (CBaseLocation.isPlayerStartLocation(Players::Enemy))
		{
			m_playerStartingCBaseLocations[Players::Enemy] = &CBaseLocation;
		}
	}

	// construct the map of tile positions to base locations
	for (float x=0; x < m_bot.Map().width(); ++x)
	{
		for (int y=0; y < m_bot.Map().height(); ++y)
		{
			for (auto & CBaseLocation : m_CBaseLocationData)
			{
				sc2::Point2D pos(x + 0.5f, y + 0.5f);
				if (CBaseLocation.containsPosition(pos))
				{
					m_tileCBaseLocations[(int)x][(int)y] = &CBaseLocation;
					
					break;
				}
			}
		}
	}

	// construct the sets of occupied base locations
	m_occupiedCBaseLocations[Players::Self] = set<const CBaseLocation *>();
	m_occupiedCBaseLocations[Players::Enemy] = set<const CBaseLocation *>();
	//We know at least one of our 
}

void CBaseLocationManager::onFrame()
{
	drawCBaseLocations();
	// reset the player occupation information for each location
	for (auto & CBaseLocation : m_CBaseLocationData)
	{
		CBaseLocation.setPlayerOccupying(Players::Self, false);
		CBaseLocation.setPlayerOccupying(Players::Enemy, false);
		CBaseLocation.resetNumEnemyCombatUnits();
	}

	// for each unit on the map, update which base location it may be occupying

	//We start with the enemy to avoid situation with proxys locations declared as enemy region
	// update enemy base occupations
	for (const auto & unit : m_bot.UnitInfo().getUnits(Players::Enemy))
	{
		if (!unit->isBuilding() || !unit->isAlive())
		{
			continue;
		}

		CBaseLocation * CBaseLocation = getCBaseLocation(unit->getPos());


		if (CBaseLocation != nullptr)
		{
			CBaseLocation->setPlayerOccupying(Players::Enemy, true);
		}
	}
	//We only really occupy a base when there is a CC
	for (const auto & unit : m_bot.UnitInfo().getUnits(Players::Self,Util::getTownHallTypes()))
	{
		// we only care about buildings on the ground
		if (!unit->isBuilding() || unit->isFlying()|| !unit->isAlive())
		{
			continue;
		}

		CBaseLocation * CBaseLocation = getCBaseLocation(unit->getPos());
		//auto test = m_bot.Map().getHeight(CBaseLocation->getCenterOfBase());
		//auto test2 = m_bot.Map().getHeight(unit->getPos());
		if (CBaseLocation != nullptr && m_bot.Map().getHeight(CBaseLocation->getCenterOfBase()) == m_bot.Map().getHeight(unit->getPos()))
		{
			CBaseLocation->setPlayerOccupying(Players::Self, true);
			CBaseLocation->setPlayerOccupying(Players::Enemy, false);
		}
	}



	// update the starting locations of the enemy player
	// this will happen one of two ways:
	
	// 1. we've seen the enemy base directly, so the CBaseLocation will know
	if (m_playerStartingCBaseLocations[Players::Enemy] == nullptr)
	{
		for (const auto & CBaseLocation : m_CBaseLocationData)
		{
			if (CBaseLocation.isPlayerStartLocation(Players::Enemy))
			{
				m_playerStartingCBaseLocations[Players::Enemy] = &CBaseLocation;
			}
		}
	}
	
	// 2. we've explored every other start location and haven't seen the enemy yet
	if (m_playerStartingCBaseLocations[Players::Enemy] == nullptr)
	{
		int numStartLocations = (int)getStartingCBaseLocations().size();
		int numExploredLocations = 0;
		CBaseLocation * unexplored = nullptr;

		for (auto & CBaseLocation : m_CBaseLocationData)
		{
			if (!CBaseLocation.isStartLocation())
			{
				continue;
			}

			if (CBaseLocation.isExplored())
			{
				numExploredLocations++;
			}
			else
			{
				unexplored = &CBaseLocation;
			}
		}

		// if we have explored all but one location, then the unexplored one is the enemy start location
		if (numExploredLocations == numStartLocations - 1 && unexplored != nullptr)
		{
			m_playerStartingCBaseLocations[Players::Enemy] = unexplored;
			unexplored->setPlayerOccupying(Players::Enemy, true);
		}
	}

	// update the occupied base locations for each player
	m_occupiedCBaseLocations[Players::Self] = set<const CBaseLocation *>();
	m_occupiedCBaseLocations[Players::Enemy] = set<const CBaseLocation *>();
	for (const auto & CBaseLocation : m_CBaseLocationData)
	{
		if (CBaseLocation.isOccupiedByPlayer(Players::Self))
		{
			m_occupiedCBaseLocations[Players::Self].insert(&CBaseLocation);
		}

		if (CBaseLocation.isOccupiedByPlayer(Players::Enemy))
		{
			m_occupiedCBaseLocations[Players::Enemy].insert(&CBaseLocation);
		}
	}

	// We want to assign the number of enemy combat units to each base to determine which one is the safest to attack
	for (const auto & unit : m_bot.UnitInfo().getUnits(Players::Enemy))
	{
		if (!unit->isCombatUnit() || !unit->isAlive())
		{
			continue;
		}

		CBaseLocation * CBaseLocation = getCBaseLocation(unit->getPos());



		if (CBaseLocation != nullptr && CBaseLocation->isOccupiedByPlayer(Players::Enemy))
		{
			CBaseLocation->incrementNumEnemyCombatUnits();
		}
	}
}

CBaseLocation * CBaseLocationManager::getCBaseLocation(const sc2::Point2D & pos) const
{
	if (!m_bot.Map().isValid(pos)) { return nullptr; }

	return m_tileCBaseLocations[(int)pos.x][(int)pos.y];
}

void CBaseLocationManager::drawCBaseLocations()
{
	if (!m_bot.Config().DrawCBaseLocationInfo)
	{
		return;
	}

	for (auto & CBaseLocation : m_CBaseLocationData)
	{
		CBaseLocation.draw();
	}

	// draw a purple sphere at the next expansion location
	sc2::Point2D nextExpansionPosition = getNextExpansion(Players::Self);

	Drawing::drawSphere(m_bot, nextExpansionPosition, 1, sc2::Colors::Purple);
	Drawing::drawText(m_bot, nextExpansionPosition, "Next Expansion Location", sc2::Colors::Purple);
}

const vector<const CBaseLocation *> & CBaseLocationManager::getCBaseLocations() const
{
	return m_CBaseLocationPtrs;
}

const vector<const CBaseLocation *> & CBaseLocationManager::getStartingCBaseLocations() const
{
	return m_startingCBaseLocations;
}

const CBaseLocation * CBaseLocationManager::getPlayerStartingCBaseLocation(int player) const
{
	return m_playerStartingCBaseLocations.at(player);
}

const CBaseLocation * CBaseLocationManager::getNaturalExpansion(int player) const
{
	const CBaseLocation * homeBase = getPlayerStartingCBaseLocation(player);
	if (!homeBase)
	{
		return nullptr;
	}
	const CBaseLocation * naturalBase = nullptr;
	int minDistance = numeric_limits<int>::max();
	for (const auto & base : getCBaseLocations())
	{
		int dist = homeBase->getGroundDistance(base->getCenterOfBase());
		if (base->getBaseID() != homeBase->getBaseID() && minDistance > dist)
		{
			minDistance = dist;
			naturalBase = base;
		}
	}
	return naturalBase;
}

const set<const CBaseLocation *> & CBaseLocationManager::getOccupiedCBaseLocations(int player) const
{
	return m_occupiedCBaseLocations.at(player);
}

const bool CBaseLocationManager::isOccupiedBy(int player, sc2::Point2D pos) const
{
	const auto base = getCBaseLocation(pos);
	return base && base->isOccupiedByPlayer(player);
}

const sc2::Point2D CBaseLocationManager::getBuildingLocation() const
{
	// First guess
	const CBaseLocation * home = m_bot.Bases().getPlayerStartingCBaseLocation(Players::Self);
	if (home && home->isOccupiedByPlayer(Players::Self))
	{
		return home->getCenterOfBase();
	}
	// uh oh
	else
	{
		float maxDist = 0.0f;
		const CBaseLocation * newHome = nullptr;
		for (const auto & base : m_bot.Bases().getOccupiedCBaseLocations(Players::Self))
		{
			for (const auto & enemy : m_bot.UnitInfo().getUnits(Players::Enemy))
			{
				const float dist = Util::DistSq(base->getCenterOfBase(), enemy->getPos());
				if (!newHome || maxDist < dist)
				{
					newHome = base;
					maxDist = dist;
				}
			}
		}
		if (newHome)
		{
			return newHome->getCenterOfBase();
		}
	}
	return m_bot.GetStartLocation();
}
const sc2::Point2D CBaseLocationManager::getRallyPoint() const
{
	// GET NEWEST EXPANSION
	sc2::Point2D fixpoint = getNewestExpansion(Players::Self);
	// Or bunker
	const CUnits bunker = m_bot.UnitInfo().getUnits(Players::Self, sc2::UNIT_TYPEID::TERRAN_BUNKER);

	vector<const CBaseLocation *> startingBases = getStartingCBaseLocations();
	sc2::Point2D targetPos(0.0f, 0.0f);
	for (const auto & base : startingBases)
	{
		targetPos+=base->getCenterOfBase();
	}
	targetPos /= static_cast<float>(startingBases.size());

	bool bunkerRally = false;
	for (const auto & b : bunker)
	{
		if (Util::Dist(fixpoint, targetPos) > Util::Dist(b->getPos(), targetPos))
		{
			fixpoint = b->getPos();
			bunkerRally = true;
		}
	}
	if (bunkerRally)
	{
		return fixpoint;
	}
	const sc2::Point2D rallyPoint = fixpoint + Util::normalizeVector(targetPos - fixpoint, 5.0f);

	// get the precomputed vector of tile positions which are sorted closes to this location
	for (const auto & pos : m_bot.Map().getClosestTilesTo(rallyPoint))
	{
		if (m_bot.Map().isWalkable(pos))
		{
			return pos;
		}
	}
	return rallyPoint;
}

sc2::Point2D CBaseLocationManager::getNextExpansion(int player) const
{
	const CBaseLocation * homeBase = getPlayerStartingCBaseLocation(player);
	const CBaseLocation * EnemyBase = getPlayerStartingCBaseLocation(Players::Enemy);
	const CBaseLocation * closestBase = nullptr;
	int minDistance = numeric_limits<int>::max();

	if (!homeBase)
	{
		return sc2::Point2D(0.0f, 0.0f);
	}
	for (const auto & base : getCBaseLocations())
	{
		// skip mineral only and starting locations (TODO: fix this)
		if (base->isMineralOnly() || base->isOccupiedByPlayer(Players::Self) || base->isOccupiedByPlayer(Players::Enemy))
		{
			continue;
		}

		// get the tile position of the base
		auto tile = base->getCenterOfBase();

		// the base's distance from our main nexus
		int distanceFromHome = homeBase->getGroundDistance(tile);

		// if it is not connected, continue
		if (distanceFromHome < 0)
		{
			continue;
		}
		int distanceFromEnemy;
		if (player == Players::Self && EnemyBase)
		{
			distanceFromEnemy = max(0, EnemyBase->getGroundDistance(tile));
		}
		else
		{
			distanceFromEnemy = 0;
		}
		if (!closestBase || distanceFromHome-0.5f*distanceFromEnemy < minDistance)
		{
			bool tooDangerous = false;
			for (const auto & enemy : m_bot.UnitInfo().getUnits(Players::Enemy))
			{
				if (enemy->isVisible() && enemy->isCombatUnit() && !enemy->isBurrowed() && Util::DistSq(enemy->getPos(), base->getCenterOfBase()) < 100.0f)
				{
					tooDangerous = true;
					break;
				}
			}
			if (!tooDangerous)
			{
				closestBase = base;
				minDistance = static_cast<int>(distanceFromHome - 0.5f*distanceFromEnemy);
			}
		}
	}

	return closestBase ? closestBase->getCenterOfBase() : sc2::Point2D(0.0f, 0.0f);
}

sc2::Point2D CBaseLocationManager::getNewestExpansion(int player) const
{
	const CBaseLocation * homeBase = getPlayerStartingCBaseLocation(player);
	if (!homeBase)
	{
		return sc2::Point2D(0.0f, 0.0f);
	}
	const CBaseLocation * newestBase = nullptr;
	float maxDistance = -1;
	for (const auto & base : getCBaseLocations())
	{
		float dist = Util::Dist(homeBase->getCenterOfBase(), base->getCenterOfBase());
		if (base->isOccupiedByPlayer(player) && maxDistance < dist)
		{
			maxDistance = dist;
			newestBase = base;
		}
	}
	if (newestBase)
	{
		return newestBase->getCenterOfBase();
	}
	else
	{
		return sc2::Point2D(0.0f, 0.0f);
	}
}

void CBaseLocationManager::assignTownhallToBase(const CUnit_ptr townHall) const
{
	CBaseLocation * CBaseLocation = getCBaseLocation(townHall->getPos());
	if (CBaseLocation)
	{
		CBaseLocation->setTownHall(townHall);
	}
}