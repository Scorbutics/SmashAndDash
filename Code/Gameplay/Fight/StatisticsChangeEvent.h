#pragma once

#include "../../ska/ECS/ECSDefines.h"
#include "../Data/RawStatistics.h"

class StatisticsChangeEvent {
public:
	StatisticsChangeEvent(const ska::EntityId& entity, RawStatistics<int>& stats, const ska::EntityId& opponent) : 
		m_entity(entity),
		m_stats(stats),
		m_opponent(opponent) {
	}
	~StatisticsChangeEvent() = default;


	const ska::EntityId& getEntity() const {
		return m_entity;
	}

	const ska::EntityId& getOpponent() const {
		return m_opponent;
	}

	RawStatistics<int>& getStats() const {
		return m_stats;
	}

	void operator=(const StatisticsChangeEvent&) = delete;

private:
	const ska::EntityId& m_entity;
	RawStatistics<int>& m_stats;
	const ska::EntityId& m_opponent;

};
