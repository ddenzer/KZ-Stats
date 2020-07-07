#include "client.h"

#include "pmove.h"

#include "stats.h"
#include "bhop.h"
#include "gstrafe.h"

bool FirstFrame = false;

GameInfo_t BuildInfo;

void ConsolePrintColor(BYTE R, BYTE G, BYTE B, const char* fmt, ...) {
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, va_alist);
	va_end(va_alist);
	TColor24 DefaultColor; PColor24 Ptr; Ptr = Console_TextColor; DefaultColor = *Ptr; Ptr->R = R; Ptr->G = G; Ptr->B = B; g_Engine.Con_Printf(buf); *Ptr = DefaultColor;
}

void InitHack()
{
	font.InitText("Courier New", 25, 10);

	ConsolePrintColor(0, 255, 0, "\n\n\t\t\tKZ Stats\n");
	ConsolePrintColor(0, 255, 0, "\t\t\tBuild date: %s (v. %s)\n", BUILD_DATE, VERSION);
	ConsolePrintColor(0, 255, 0, "\t\t\tCredits: FAME\n");

	Stats::ResetStats();
	Bhop::ResetStats();
	Gstrafe::ResetStats();
}

void HUD_Frame( double time )
{
	if ( !FirstFrame )
	{
		g_Screen.iSize = sizeof( SCREENINFO );

		offset.HLType = g_Studio.IsHardware() + 1;

		offset.ConsoleColorInitalize();

		InitHack();

		FirstFrame = true;
	}

	g_Engine.pfnGetScreenInfo( &g_Screen );
	g_Client.HUD_Frame( time );
}

void HUD_Redraw( float time , int intermission )
{
	g_Client.HUD_Redraw(time, intermission);

	if (!g_Stats.m_enabled)
	{
		return;
	}

	if (g_Engine.Sys_FloatTime() > g_Stats.m_show_time || !g_Stats.m_show_time || g_Stats.m_stats_type == Stats::StatsType::StatsNone)
	{
		return;
	}

	char show_stats[32];
	int rgb[3];

	if (g_Stats.m_stats_type == Stats::StatsType::StatsBhop)
	{
		sprintf_s(show_stats, "%d [%s]", g_BhopStats.m_fog, Bhop::GetPrefixType());

		if (g_BhopStats.m_bhop_type == 1 || g_BhopStats.m_bhop_type == 2)
		{
			rgb[0] = 0;
			rgb[1] = 255;
			rgb[2] = 0;
		}
		else
		{
			rgb[0] = 255;
			rgb[1] = 0;
			rgb[2] = 0;
		}
	}
	else if (g_Stats.m_stats_type == Stats::StatsType::StatsGstrafe)
	{
		sprintf_s(show_stats, "%d [%s]", g_GstrafeStats.m_fog, Gstrafe::GetPrefixType());

		if (g_GstrafeStats.m_gstrafe_type == 1 || g_GstrafeStats.m_gstrafe_type == 2)
		{
			rgb[0] = 0;
			rgb[1] = 255;
			rgb[2] = 0;
		}
		else
		{
			rgb[0] = 255;
			rgb[1] = 0;
			rgb[2] = 0;
		}
	}

	char show_pre[32];
	sprintf_s(show_pre, "%0.2f ", g_Stats.m_pre);

	font.Print(g_Screen.iWidth / 2, g_Screen.iHeight / 1.57, 0, 255, 0, 255, FL_CENTER | FL_OUTLINE, show_pre);
	font.Print(g_Screen.iWidth / 2, g_Screen.iHeight / 1.63, rgb[0], rgb[1], rgb[2], 255, FL_CENTER | FL_OUTLINE, show_stats);
}

void CL_CreateMove( float frametime , usercmd_s *cmd , int active )
{
	g_Client.CL_CreateMove(frametime, cmd, active);

	if (!g_Stats.m_enabled)
	{
		return;
	}

	auto buttons = cmd->buttons;
	static int old_buttons;

	auto velocity = g_PMove.m_velocity;
	velocity[2] = 0.0;
	float speed = velocity.Length();
	static float old_speed;
	auto maxspeed = g_PMove.m_maxspeed;
	auto max_pre_strafe = maxspeed * 1.2;

	static int fog;

	if (g_PMove.m_flags & FL_ONGROUND)
	{
		fog++;

		if (fog == 1)
		{
			g_GstrafeStats.m_type = (g_PMove.m_flags & FL_DUCKING) ? Gstrafe::GstrafeType::StandUp : Gstrafe::GstrafeType::Regular;
		}

		if (fog <= 5)
		{
			if (buttons & IN_JUMP && ~old_buttons & IN_JUMP)
			{
				g_Stats.m_stats_type = Stats::StatsType::StatsBhop;
				if (g_BhopStats.m_bhops + 1 > 100)
				{
					Bhop::ResetStats();
				}

				g_BhopStats.m_bhops++;
				g_BhopStats.m_fog = fog;

				g_BhopStats.m_bhop_type = Bhop::GetType(g_BhopStats.m_fog, buttons, old_buttons, speed, old_speed, max_pre_strafe);

				if (g_BhopStats.m_bhop_type == 1)
				{
					g_BhopStats.m_perfect_bhops++;
				}

				g_Stats.m_pre = speed;
				g_Stats.m_show_time = g_Engine.Sys_FloatTime() + 3;

				ConsolePrintColor(0, 255, 0, "[KZ Stats] Total Bhops: %d | Perfect bhops: %d | FOG: %d [%s] | Speed: %0.2f\n",
					g_BhopStats.m_bhops, g_BhopStats.m_perfect_bhops, g_BhopStats.m_fog, Bhop::GetPrefixType(), g_Stats.m_pre);
			}
			else if (buttons & IN_DUCK && ~old_buttons & IN_DUCK)
			{
				g_Stats.m_stats_type = Stats::StatsType::StatsGstrafe;
				if (g_GstrafeStats.m_gstrafes + 1 > 100)
				{
					Gstrafe::ResetStats();
				}

				g_GstrafeStats.m_gstrafes++;
				g_GstrafeStats.m_fog = fog;

				g_GstrafeStats.m_gstrafe_type = Gstrafe::GetType();

				g_Stats.m_pre = speed;
				g_Stats.m_show_time = g_Engine.Sys_FloatTime() + 3;

				ConsolePrintColor(0, 255, 0, "[KZ Stats] Total Gstrafes: %d | FOG: %d [%s] | Speed: %0.2f\n",
					g_GstrafeStats.m_gstrafes, g_GstrafeStats.m_fog, Gstrafe::GetPrefixType(), g_Stats.m_pre);
			}
		}
	}
	else
	{
		if (fog)
		{
			fog = 0;
		}
	}

	old_speed = speed;
	old_buttons = buttons;
}

int HUD_Key_Event( int down , int keynum , const char *pszCurrentBinding )
{
	if (keynum == K_INS)
	{
		g_Stats.m_enabled = !g_Stats.m_enabled;
		ConsolePrintColor(255, 0, 0, "[KZ Stats] Stats %s\n", g_Stats.m_enabled ? "enabled" : "disabled");
	}

	return g_Client.HUD_Key_Event(down, keynum, pszCurrentBinding);
}

void HUD_PlayerMove(struct playermove_s* ppmove, int server)
{
	g_Client.HUD_PlayerMove(ppmove, server);

	g_PMove.m_velocity = ppmove->velocity;
	g_PMove.m_maxspeed = ppmove->maxspeed;
	g_PMove.m_flags = ppmove->flags;
}

void HookFunction()
{
	g_pClient->HUD_Frame = HUD_Frame;
	g_pClient->HUD_Redraw = HUD_Redraw;
	g_pClient->CL_CreateMove = CL_CreateMove;
	g_pClient->HUD_Key_Event = HUD_Key_Event;
	g_pClient->HUD_PlayerMove = HUD_PlayerMove;
}