#pragma once

#include "main.h"

class Bhop
{
public:
	static void ResetStats();
	static int GetType(int fog, int buttons, int old_buttons, float speed, float old_speed, float max_pre_strafe);
	static char* GetPrefixType();

	int m_bhops;
	int m_perfect_bhops;
	int m_fog;
	int m_bhop_type;
};

extern Bhop g_BhopStats;