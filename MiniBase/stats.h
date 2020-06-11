#pragma once
class Stats
{
public:
	enum class StatsType
	{
		StatsNone = -1,
		StatsBhop,
		StatsGstrafe
	};

	static void ResetStats();

	StatsType m_stats_type;
	float m_show_time;
	float m_pre;
};

extern Stats* g_Stats;