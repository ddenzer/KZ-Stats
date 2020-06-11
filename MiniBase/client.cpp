#include "client.h"

#include "stats.h"
#include "bhop.h"
#include "gstrafe.h"

bool FirstFrame = false;

GameInfo_t BuildInfo;

void HookUserMessages()
{
	pUserMsgBase = (PUserMsg)offset.FindUserMsgBase();

	pResetHUD = HookUserMsg( "ResetHUD" , ResetHUD );
	pSetFOV = HookUserMsg( "SetFOV" , SetFOV );
	pTeamInfo = HookUserMsg( "TeamInfo" , TeamInfo );
	pCurWeapon = HookUserMsg( "CurWeapon" , CurWeapon );
	pDeathMsg = HookUserMsg( "DeathMsg" , DeathMsg );
}

void HookEngineMessages()
{
	pEngineMsgBase = (PEngineMsg)offset.FindSVCMessages();

	pSVC_sound = HookEngineMsg( "svc_sound" , SVC_Sound );
	pSVC_StuffText = HookEngineMsg( "svc_stufftext" , SVC_StuffText );
	pSVC_NewUserMsg = HookEngineMsg( "svc_newusermsg" , SVC_NewUserMsg );
	pSVC_UpdateUserInfo = HookEngineMsg( "svc_updateuserinfo" , SVC_UpdateUserInfo );
	pSVC_SendCvarValue = HookEngineMsg( "svc_sendcvarvalue" , SVC_SendCvarValue );
	pSVC_SendCvarValue2 = HookEngineMsg( "svc_sendcvarvalue2" , SVC_SendCvarValue2 );
	pSVC_Director = HookEngineMsg( "svc_director" , SVC_Director );
}

/*void ConsolePrintColor( BYTE R , BYTE G , BYTE B , char* string )
{
	TColor24 DefaultColor;
	PColor24 Ptr;
	Ptr = Console_TextColor;
	DefaultColor = *Ptr;
	Ptr->R = R;
	Ptr->G = G;
	Ptr->B = B;
	g_Engine.Con_Printf( "%s" , string );
	*Ptr = DefaultColor;
}*/

void ConsolePrintColor(BYTE R, BYTE G, BYTE B, const char* fmt, ...) {
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, va_alist);
	va_end(va_alist);
	TColor24 DefaultColor; PColor24 Ptr; Ptr = Console_TextColor; DefaultColor = *Ptr; Ptr->R = R; Ptr->G = G; Ptr->B = B; g_Engine.Con_Printf(buf); *Ptr = DefaultColor;
}

void DumpEngineMessages()
{
	PEngineMsg Ptr = nullptr;
	Ptr = pEngineMsgBase;

	while ( native_strcmp( Ptr->name , OFF_SVC_MESSAGES_P ) )
	{
		//g_Engine.Con_Printf( "[%i] EngineMsgName: %s , Ptr: 0x%X\n" , Ptr->number , Ptr->name , Ptr->pfn );
		Ptr++;
	}
}

char* ConvertTypeToRenderString( byte type )
{
	char* RenderTypeGL = "opengl";
	char* RenderTypeD3 = "d3d";
	char* RenderTypeSF = "software";

	if ( type == 3 )
		return RenderTypeD3;
	else if ( type == 2 )
		return RenderTypeGL;
	else if ( type == 1 )
		return RenderTypeSF;

	return "unknown";
}

void InitHack()
{
	font.InitText("Courier New", 25, 10);

	ConsolePrintColor(0, 255, 0, "\n\n\t\t\tKZ Stats\n");
	ConsolePrintColor(0, 255, 0, "\t\t\tBuild date: %s (v. %s)\n", BUILD_DATE, VERSION);
	ConsolePrintColor(0, 255, 0, "\t\t\tCredits: FAME\n");

	/*ConsolePrintColor( 255 , 64 , 64 , "\n\n\t\t\t[visit: csxr.ru]\n" );

	ConsolePrintColor( 0 , 164 , 255 , "\t\t\t[render: " );
	ConsolePrintColor( 0 , 164 , 255 , ConvertTypeToRenderString( offset.HLType ) );
	ConsolePrintColor( 0 , 164 , 255 , "]\n" );

	ConsolePrintColor( 255 , 200 , 0 , "\t\t\t[by _or_75]\n\n" );

	g_Engine.Con_Printf( "\t\t\tclient: 0x%X\n" , g_pClient );
	g_Engine.Con_Printf( "\t\t\tengine: 0x%X\n" , g_pEngine );
	g_Engine.Con_Printf( "\t\t\tstudio: 0x%X\n\n" , g_pStudio );

	g_Engine.Con_Printf( "\t\t\tuser_msg: 0x%X\n" , pUserMsgBase );
	g_Engine.Con_Printf( "\t\t\tengine_msg: 0x%X\n\n" , pEngineMsgBase );
	
	g_Engine.Con_Printf( "\t\t\tGameName: %s\n" , BuildInfo.GameName );
	g_Engine.Con_Printf( "\t\t\tGameVersion: %s\n" , BuildInfo.GameVersion );
	g_Engine.Con_Printf( "\t\t\tProtocol: %d\n" , BuildInfo.Protocol );
	g_Engine.Con_Printf( "\t\t\tBuild: %d\n\n" , BuildInfo.Build );

	g_Engine.Con_Printf( "\t\t\tbasedir: %s\n" , BaseDir );
	
	g_Engine.pfnAddCommand( "cl_dump_messages" , DumpEngineMessages );*/

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
		offset.GetGameInfo( &BuildInfo );

		HookUserMessages();
		HookEngineMessages();

		InitHack();

		FirstFrame = true;
	}

	g_Engine.pfnGetScreenInfo( &g_Screen );
	g_Client.HUD_Frame( time );
}

void HUD_Redraw(float _time , int intermission)
{
	g_Client.HUD_Redraw(_time, intermission);

	if (g_Engine.Sys_FloatTime() > g_Stats->m_show_time || !g_Stats->m_show_time || g_Stats->m_stats_type == Stats::StatsType::StatsNone)
	{
		return;
	}

	char show_stats[32];
	int rgb[3];

	if (g_Stats->m_stats_type == Stats::StatsType::StatsBhop)
	{
		if (!g_BhopStats->m_bhop_type)
		{
			return;
		}

		sprintf_s(show_stats, "%d [%s]", g_BhopStats->m_fog, Bhop::GetPrefixType());

		if (g_BhopStats->m_bhop_type == 1 || g_BhopStats->m_bhop_type == 2)
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
	else if (g_Stats->m_stats_type == Stats::StatsType::StatsGstrafe)
	{
		rgb[0] = 0;
		rgb[1] = 255;
		rgb[2] = 0;

		sprintf_s(show_stats, "%d", g_GstrafeStats->m_fog);
	}

	char show_pre[32];
	sprintf_s(show_pre, "%0.2f ", g_Stats->m_pre);

	font.Print(g_Screen.iWidth / 2, g_Screen.iHeight / 1.57, 0, 255, 0, 255, FL_CENTER, show_pre);
	font.Print(g_Screen.iWidth / 2, g_Screen.iHeight / 1.63, rgb[0], rgb[1], rgb[2], 255, FL_CENTER, show_stats);
}

void CL_CreateMove( float frametime , usercmd_s *cmd , int active )
{
	g_Client.CL_CreateMove(frametime, cmd, active);

	auto buttons = cmd->buttons;
	static int old_buttons;

	auto velocity = g_pMove->velocity;
	velocity[2] = 0.0;
	float speed = velocity.Length();
	static float old_speed;
	auto maxspeed = g_pMove->maxspeed;
	auto max_pre_strafe = maxspeed * 1.2;

	static int fog;

	if (g_pMove->flags & FL_ONGROUND)
	{
		fog++;

		if (fog <= 5)
		{
			if (buttons & IN_JUMP && ~old_buttons & IN_JUMP)
			{
				g_Stats->m_stats_type = Stats::StatsType::StatsBhop;
				if (g_BhopStats->m_bhops + 1 > 100)
				{
					Bhop::ResetStats();
				}

				g_BhopStats->m_bhops++;
				//g_BhopStats->m_pre = speed;
				g_BhopStats->m_fog = fog;

				g_BhopStats->m_bhop_type = Bhop::GetType(g_BhopStats->m_fog, buttons, old_buttons, speed, old_speed, max_pre_strafe);

				if (g_BhopStats->m_bhop_type == 1)
				{
					g_BhopStats->m_perfect_bhops++;
				}

				ConsolePrintColor(0, 255, 0, "[KZ Stats] Total Bhops: %d | Perfect bhops: %d | FOG: %d [%s]\n",
					g_BhopStats->m_bhops, g_BhopStats->m_perfect_bhops, g_BhopStats->m_fog, Bhop::GetPrefixType());

				g_Stats->m_pre = speed;
				g_Stats->m_show_time = g_Engine.Sys_FloatTime() + 3;
			}
			else if (buttons & IN_DUCK && ~old_buttons & IN_DUCK)
			{
				g_Stats->m_stats_type = Stats::StatsType::StatsGstrafe;
				if (g_GstrafeStats->m_gstrafes + 1 > 100)
				{
					Gstrafe::ResetStats();
				}

				g_GstrafeStats->m_gstrafes++;
				g_GstrafeStats->m_fog = fog;

				ConsolePrintColor(0, 255, 0, "[KZ Stats] Total Gstrafes: %d | FOG: %d\n", g_GstrafeStats->m_gstrafes, g_GstrafeStats->m_fog);

				g_Stats->m_pre = speed;
				g_Stats->m_show_time = g_Engine.Sys_FloatTime() + 3;
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

void HookFunction()
{
	g_pClient->HUD_Frame = HUD_Frame;
	g_pClient->HUD_Redraw = HUD_Redraw;
	g_pClient->CL_CreateMove = CL_CreateMove;
	//g_pClient->HUD_PlayerMove = HUD_PlayerMove;
}