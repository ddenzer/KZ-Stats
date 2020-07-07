#include "bhop.h"

Bhop g_BhopStats;

void Bhop::ResetStats()
{
	g_BhopStats.m_bhops = 0;
	g_BhopStats.m_perfect_bhops = 0;
	g_BhopStats.m_fog = -1;
	g_BhopStats.m_bhop_type = -1;
}

// FAME
int Bhop::GetType(int fog, int buttons, int old_buttons, float speed, float old_speed, float max_pre_strafe)
{
    if (fog && fog > 5 && ~buttons & IN_JUMP && old_buttons & IN_JUMP)
    {
        return 0;
    }
    if (speed < max_pre_strafe && (fog == 1 || (fog >= 2 && old_speed > max_pre_strafe)))
    {
        return 1;
    }
    if (fog == 1)
    {
        return 2;
    }
    if (fog == 2)
    {
        return 3;
    }
    return 4;
}

char* Bhop::GetPrefixType()
{
    switch (g_BhopStats.m_bhop_type)
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