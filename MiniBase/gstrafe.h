#pragma once
class Gstrafe
{
public:
	enum class GstrafeType
	{
		None = 0,
		Regular,
		StandUp
	};
	static void ResetStats();
	static int GetType();
	static char* GetPrefixType();

	int m_gstrafes;
	int m_fog;
	GstrafeType m_type;
	int m_gstrafe_type;
};

extern Gstrafe g_GstrafeStats;