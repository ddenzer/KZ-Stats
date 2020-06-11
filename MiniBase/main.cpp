#include "main.h"

#include "stats.h"
#include "bhop.h"
#include "gstrafe.h"

playermove_t  *g_pMove = nullptr;
cl_clientfunc_t *g_pClient = nullptr;
cl_enginefunc_t *g_pEngine = nullptr;
engine_studio_api_t *g_pStudio = nullptr;

cl_clientfunc_t g_Client;
cl_enginefunc_t g_Engine;
engine_studio_api_t g_Studio;

PUserMsg pUserMsgBase;
PEngineMsg pEngineMsgBase;
PColor24 Console_TextColor;

SCREENINFO g_Screen;

BYTE bPreType = 0;

ofstream logfile;
char* BaseDir;

DWORD WINAPI CheatEntry( LPVOID lpThreadParameter );

void add_log( const char *fmt , ... )
{
	if ( !fmt )	return;
	va_list va_alist;
	char logbuf[256] = { 0 };
	va_start( va_alist , fmt );
	vsnprintf( logbuf + native_strlen( logbuf ) , sizeof( logbuf ) - native_strlen( logbuf ) , fmt , va_alist );
	va_end( va_alist );
	logfile << logbuf << endl;
}

DWORD WINAPI ProcessReload( LPVOID lpThreadParameter )
{
	while ( true )
	{
		if ( FirstFrame )
		{
			offset.GetRenderType();

			if ( !offset.GetModuleInfo() )
				FirstFrame = false;
		}
		else
		{
			CreateThread( 0 , 0 , CheatEntry , 0 , 0 , 0 );
		}

		Sleep( 100 );
	}

	return 0;
}

DWORD WINAPI CheatEntry( LPVOID lpThreadParameter )
{
	static HANDLE hProcessReloadThread = 0;

	if ( hProcessReloadThread )
	{
		TerminateThread( hProcessReloadThread , 0 );
		CloseHandle( hProcessReloadThread );
	}

	g_Stats = new Stats;
	g_BhopStats = new Bhop;
	g_GstrafeStats = new Gstrafe;
	
	BYTE counter_find = 0;

start_hook:

	if ( counter_find == 100 )
	{
		offset.Error( ERROR_FIND );
	}

	Sleep( 100 );
	counter_find++;

	if ( !offset.GetModuleInfo() )
	{
		goto start_hook;
	}

	DWORD ClientTable = offset.FindClientTable();

	if ( ClientTable )
	{
		g_pClient = (cl_clientfunc_t*)ClientTable;
		offset.CopyClient();

		if ( (DWORD)g_Client.Initialize )
		{
			DWORD EngineTable = offset.FindEngineTable();

			if ( EngineTable )
			{
				g_pEngine = (cl_enginefunc_t*)EngineTable;
				offset.CopyEngine();

				if ( (DWORD)g_Engine.V_CalcShake )
				{
					DWORD StudioTable = offset.FindStudioTable();

					if ( StudioTable )
					{
						g_pStudio = (engine_studio_api_t*)StudioTable;
						offset.CopyStudio();

						if ( (DWORD)g_Studio.StudioSetupSkin )
						{
							PVOID PlayerMove = offset.FindPlayerMove();

							if (PlayerMove)
							{
								g_pMove = (playermove_t*)PlayerMove;
								while (!FirstFrame)
								{
									HookFunction();
									Sleep(100);
								}

								bPreType = offset.HLType;

								hProcessReloadThread = CreateThread(0, 0, ProcessReload, 0, 0, 0);
							}
							else
							{
								goto start_hook;
							}
						}
						else
						{
							goto start_hook;
						}
					}
					else
					{
						goto start_hook;
					}
				}
				else
				{
					goto start_hook;
				}
			}
			else
			{
				goto start_hook;
			}
		}
		else
		{
			goto start_hook;
		}
	}
	else
	{
		goto start_hook;
	}

	return 0;
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL , DWORD fdwReason , LPVOID lpReserved )
{
	switch ( fdwReason )
	{
	case DLL_PROCESS_ATTACH:
	{

		HANDLE hMutextBase = CreateMutexA(0, true, BASE_MUTEX);

		if (GetLastError() == ERROR_ALREADY_EXISTS)
			return TRUE;

		DisableThreadLibraryCalls(hinstDLL);

		/*BaseDir = (char*)HeapAlloc( GetProcessHeap() , HEAP_ZERO_MEMORY , MAX_PATH );
		char* LogFile = new char[256];

		GetModuleFileNameA( hinstDLL , BaseDir , MAX_PATH );

		char* pos = BaseDir + native_strlen( BaseDir );
		while ( pos >= BaseDir && *pos != '\\' ) --pos; pos[1] = 0;

		lstrcpyA( LogFile , BaseDir );
		lstrcatA( LogFile , "\\log_server.txt" );

		remove( LogFile );
		logfile.open( LogFile , ios::app );*/

		CreateThread(0, 0, CheatEntry, 0, 0, 0);

		return TRUE;
	}
	case DLL_PROCESS_DETACH:
	{
		delete g_Stats;
		g_Stats = nullptr;

		delete g_BhopStats;
		g_BhopStats = nullptr;
		
		delete g_GstrafeStats;
		g_GstrafeStats = nullptr;
		return TRUE;
	}
	}

	return FALSE;
}

extern "C" __declspec(dllexport) BOOL WINAPI RIB_Main(LPVOID lp, LPVOID lp2, LPVOID lp3, LPVOID lp4, LPVOID lp5)
{
	return TRUE;
}