#include "stats.h"

Stats g_Stats;

void Stats::ResetStats()
{
	g_Stats.m_enabled = true;
	g_Stats.m_stats_type = Stats::StatsType::StatsNone;
	g_Stats.m_show_time = 0.f;
	g_Stats.m_pre = 0.f;
}