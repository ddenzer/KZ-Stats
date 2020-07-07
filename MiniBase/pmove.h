#pragma once
#include "main.h"
class PMove
{
public:
	Vector m_velocity;
	float m_maxspeed;
	int m_flags;
};

extern PMove g_PMove;