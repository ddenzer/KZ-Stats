#pragma once
class Gstrafe
{
public:
	static void ResetStats();

	int m_gstrafes;
	int m_fog;
};

extern Gstrafe* g_GstrafeStats;