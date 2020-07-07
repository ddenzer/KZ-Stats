#include "gstrafe.h"

Gstrafe g_GstrafeStats;

void Gstrafe::ResetStats()
{
	g_GstrafeStats.m_gstrafes = 0;
	g_GstrafeStats.m_fog = -1;
	g_GstrafeStats.m_gstrafe_type = 0;
}

int Gstrafe::GetType()
{
	if (g_GstrafeStats.m_type == Gstrafe::GstrafeType::None)
	{
		return 0;
	}
	else if (g_GstrafeStats.m_type == Gstrafe::GstrafeType::Regular)
	{
		switch (g_GstrafeStats.m_fog)
		{
		case 1:
		{
			return 1;
		}
		case 2:
		{
			return 2;
		}
		case 3:
		{
			return 3;
		}
		}
	}
	else if (g_GstrafeStats.m_type == Gstrafe::GstrafeType::StandUp)
	{
		switch (g_GstrafeStats.m_fog)
		{
		case 1:
		{
			return 2;
		}
		case 2:
		{
			return 1;
		}
		case 3:
		{
			return 3;
		}
		}
	}

	return 4;
}

char* Gstrafe::GetPrefixType()
{
	switch (g_GstrafeStats.m_gstrafe_type)
	{
	case 0:
	{
		return "NaN";
	}
	case 1:
	{
		return "P";
	}
	case 2:
	{
		return "G";
	}
	case 3:
	{
		return "B";
	}
	case 4:
	{
		return "VB";
	}
	}
}