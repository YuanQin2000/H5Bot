#pragma once

#include "sc2api/sc2_api.h"
#include "CUnit.h"
#include "CBaseLocation.h"

class CCBot;

class CBaseLocationManager
{
	CCBot & m_bot;

	vector<CBaseLocation>						m_CBaseLocationData;
	vector<const CBaseLocation *>				m_CBaseLocationPtrs;
	vector<const CBaseLocation *>				m_startingCBaseLocations;
	map<int, const CBaseLocation *>				m_playerStartingCBaseLocations;
	map<int, set<const CBaseLocation *>>	m_occupiedCBaseLocations;
	vector<vector<CBaseLocation *>>		m_tileCBaseLocations;

public:

	CBaseLocationManager(CCBot & bot);
	
	void onStart();
	void onFrame();
	void drawCBaseLocations();

	const vector<const CBaseLocation *> & getCBaseLocations() const;
	CBaseLocation * getCBaseLocation(const sc2::Point2D & pos) const;
	const vector<const CBaseLocation *> & getStartingCBaseLocations() const;
	const set<const CBaseLocation *> & getOccupiedCBaseLocations(int player) const;
	const bool isOccupiedBy(int player, sc2::Point2D pos) const;
	const sc2::Point2D getBuildingLocation() const;
	const CBaseLocation * getPlayerStartingCBaseLocation(int player) const;

	const CBaseLocation * getNaturalExpansion(int player) const;
	
	sc2::Point2D getNextExpansion(int player) const;
	sc2::Point2D getNewestExpansion(int player) const;
	const sc2::Point2D getRallyPoint() const;
	void assignTownhallToBase(const CUnit_ptr townHall) const;
};
