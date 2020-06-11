#include "gstrafe.h"

Gstrafe* g_GstrafeStats;

void Gstrafe::ResetStats()
{
	g_GstrafeStats->m_gstrafes = 0;
	g_GstrafeStats->m_fog = -1;
}