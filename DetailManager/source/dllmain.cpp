﻿#include "dllmain.hpp"
wchar_t g_module_dir[MAX_PATH];

#define MAX_DETAILMODULE_NUM 64
#define USEINI 1


typedef void (WINAPI* DETAILMODULE_ATSLOAD)(void);
typedef void (WINAPI* DETAILMODULE_ATSDISPOSE)(void);
typedef int (WINAPI* DETAILMODULE_ATSGETPLUGINVERSION)(void);
typedef void (WINAPI* DETAILMODULE_ATSSETVEHICLESPEC)(ATS_VEHICLESPEC);
typedef void (WINAPI* DETAILMODULE_ATSINITIALIZE)(int);
typedef ATS_HANDLES(WINAPI* DETAILMODULE_ATSELAPSE)(ATS_VEHICLESTATE, int*, int*);
typedef void (WINAPI* DETAILMODULE_ATSSETPOWER)(int);
typedef void (WINAPI* DETAILMODULE_ATSSETBRAKE)(int);
typedef void (WINAPI* DETAILMODULE_ATSSETREVERSER)(int);
typedef void (WINAPI* DETAILMODULE_ATSKEYDOWN)(int);
typedef void (WINAPI* DETAILMODULE_ATSKEYUP)(int);
typedef void (WINAPI* DETAILMODULE_ATSHORNBLOW)(int);
typedef void (WINAPI* DETAILMODULE_ATSDOOROPEN)(void);
typedef void (WINAPI* DETAILMODULE_ATSDOORCLOSE)(void);
typedef void (WINAPI* DETAILMODULE_ATSSETSIGNAL)(int);
typedef void (WINAPI* DETAILMODULE_ATSSETBEACONDATA)(ATS_BEACONDATA);

struct ST_DETAILMODULE_ATS_DELEGATE_FUNC
{
	HMODULE hm_dll;
	DETAILMODULE_ATSLOAD atsLoad;
	DETAILMODULE_ATSDISPOSE atsDispose;
	DETAILMODULE_ATSGETPLUGINVERSION atsGetPluginVersion;
	DETAILMODULE_ATSSETVEHICLESPEC atsSetVehicleSpec;
	DETAILMODULE_ATSINITIALIZE atsInitialize;
	DETAILMODULE_ATSELAPSE atsElapse;
	DETAILMODULE_ATSSETPOWER atsSetPower;
	DETAILMODULE_ATSSETBRAKE atsSetBrake;
	DETAILMODULE_ATSSETREVERSER atsSetReverser;
	DETAILMODULE_ATSKEYDOWN atsKeyDown;
	DETAILMODULE_ATSKEYUP atsKeyUp;
	DETAILMODULE_ATSHORNBLOW atsHornBlow;
	DETAILMODULE_ATSDOOROPEN atsDoorOpen;
	DETAILMODULE_ATSDOORCLOSE atsDoorClose;
	DETAILMODULE_ATSSETSIGNAL atsSetSignal;
	DETAILMODULE_ATSSETBEACONDATA atsSetBeaconData;
	ATS_HANDLES last_handle;
};

int g_num_of_detailmodules;
ST_DETAILMODULE_ATS_DELEGATE_FUNC g_detailmodules[MAX_DETAILMODULE_NUM];

ATS_HANDLES g_handles[2];

bool g_first_time;

#include <sys/stat.h>
#include <stdio.h>


//#include "ini.h"    // #include "Ats.h"の上に書く
#include "trigger.h"


trigger g_trigger;
bool g_load_failed;

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // DLL モジュールのハンドル
	DWORD fdwReason,     // 関数を呼び出す理由
	LPVOID lpvReserved   // 予約済み
)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:

	{
		wchar_t fullpath[MAX_PATH];
		wchar_t drive[MAX_PATH],
			dir[MAX_PATH];

		GetModuleFileNameW(hinstDLL, fullpath, MAX_PATH);
		_wsplitpath_s(fullpath, drive, MAX_PATH, dir, MAX_PATH, 0, 0, 0, 0);

		wcscpy(g_module_dir, drive);
		wcscat(g_module_dir, dir);

#if USEINI == 1

#endif
	}

	break;

	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
	{
		atsDispose();
	}
	break;
	}

	return true;
}

int IniIntGet(wchar_t* section, wchar_t* key, int standard, wchar_t* dir)
{
	int exp;
	exp = GetPrivateProfileInt(section, key, standard, dir);
	return exp;
}

// Called when this plug-in is loaded
void WINAPI atsLoad()
{
	wchar_t detailmodules_txt_path[MAX_PATH];
	wchar_t detailswitcher_ini_path[MAX_PATH];

	g_first_time = true;
	g_load_failed = false;

	int ret;
	int ret2;
	{
		struct _stat buf;

		g_num_of_detailmodules = 0;
		memset(g_detailmodules, 0, sizeof(ST_DETAILMODULE_ATS_DELEGATE_FUNC) * MAX_DETAILMODULE_NUM);
		memset(g_handles, 0, sizeof(ATS_HANDLES) * 2);

		wcscpy_s(detailmodules_txt_path, g_module_dir);
		wcscpy_s(detailswitcher_ini_path, g_module_dir);
		wcscat(detailmodules_txt_path, L"\\detailmodules.txt");
		wcscat(detailswitcher_ini_path, L"\\DetailSwitcher.ini");

		ret = _wstat(detailmodules_txt_path, &buf);
		ret2 = _wstat(detailswitcher_ini_path, &buf);
	}

	if (ret)
	{
		char* p = 0; *p = 1;
		return;
	}
	if (ret2)
	{
		char* p = 0; *p = 1;
		return;
	}

	FILE* fp = _wfopen(detailmodules_txt_path, L"r");

	while (!feof(fp))
	{
		char module_path[MAX_PATH];
		wchar_t module_full_path[MAX_PATH],
			module_path_wcs[MAX_PATH];

		memset(module_path, 0, sizeof(char) * MAX_PATH);

		fgets(module_path, MAX_PATH, fp);

		if (module_path[0] =='#')
		{
			continue;
		}

		{
			char* p = strchr(module_path, 10);

			if (p)
			{
				*p = 0;
			}
		}

		wcscpy(module_full_path, g_module_dir);
		mbstowcs(module_path_wcs, module_path, MAX_PATH);
		//            wcscat(module_full_path, L"..\\..\\Plugin\\");
		wcscat(module_full_path, module_path_wcs);

		{
			struct _stat buf;
			int exists;

			exists = !_wstat(module_full_path, &buf);

			if (!exists)
			{
				const auto title = TEXT("DetailSelector");
				wchar_t body[512] = L"下記のATSプラグインが不足しています。\n\n";
				wcscat_s(body, module_path_wcs);
				MessageBox(NULL, body, title, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
				g_load_failed = true;
				break;
			}
			
			g_detailmodules[g_num_of_detailmodules].hm_dll = LoadLibrary(module_full_path);

			g_detailmodules[g_num_of_detailmodules].atsLoad = reinterpret_cast<DETAILMODULE_ATSLOAD>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "Load"));
			g_detailmodules[g_num_of_detailmodules].atsDispose = reinterpret_cast<DETAILMODULE_ATSDISPOSE>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "Dispose"));
			g_detailmodules[g_num_of_detailmodules].atsGetPluginVersion = reinterpret_cast<DETAILMODULE_ATSGETPLUGINVERSION>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "GetPluginVersion"));
			g_detailmodules[g_num_of_detailmodules].atsSetVehicleSpec = reinterpret_cast<DETAILMODULE_ATSSETVEHICLESPEC>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "SetVehicleSpec"));
			g_detailmodules[g_num_of_detailmodules].atsInitialize = reinterpret_cast<DETAILMODULE_ATSINITIALIZE>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "Initialize"));
			g_detailmodules[g_num_of_detailmodules].atsElapse = reinterpret_cast<DETAILMODULE_ATSELAPSE>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "Elapse"));
			g_detailmodules[g_num_of_detailmodules].atsSetPower = reinterpret_cast<DETAILMODULE_ATSSETPOWER>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "SetPower"));
			g_detailmodules[g_num_of_detailmodules].atsSetBrake = reinterpret_cast<DETAILMODULE_ATSSETBRAKE>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "SetBrake"));
			g_detailmodules[g_num_of_detailmodules].atsSetReverser = reinterpret_cast<DETAILMODULE_ATSSETREVERSER>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "SetReverser"));
			g_detailmodules[g_num_of_detailmodules].atsKeyDown = reinterpret_cast<DETAILMODULE_ATSKEYDOWN>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "KeyDown"));
			g_detailmodules[g_num_of_detailmodules].atsKeyUp = reinterpret_cast<DETAILMODULE_ATSKEYUP>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "KeyUp"));
			g_detailmodules[g_num_of_detailmodules].atsHornBlow = reinterpret_cast<DETAILMODULE_ATSHORNBLOW>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "HornBlow"));
			g_detailmodules[g_num_of_detailmodules].atsDoorOpen = reinterpret_cast<DETAILMODULE_ATSDOOROPEN>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "DoorOpen"));
			g_detailmodules[g_num_of_detailmodules].atsDoorClose = reinterpret_cast<DETAILMODULE_ATSDOORCLOSE>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "DoorClose"));
			g_detailmodules[g_num_of_detailmodules].atsSetSignal = reinterpret_cast<DETAILMODULE_ATSSETSIGNAL>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "SetSignal"));
			g_detailmodules[g_num_of_detailmodules].atsSetBeaconData = reinterpret_cast<DETAILMODULE_ATSSETBEACONDATA>(GetProcAddress(g_detailmodules[g_num_of_detailmodules].hm_dll, "SetBeaconData"));

			memset(&g_detailmodules[g_num_of_detailmodules].last_handle, 0, sizeof(ATS_HANDLES));

			if (g_detailmodules[g_num_of_detailmodules].atsLoad != NULL)
			{
				g_detailmodules[g_num_of_detailmodules].atsLoad();
			}

			++g_num_of_detailmodules;
		}
	}

	fclose(fp);

	//INIファイル読み込み
	g_trigger.ini_ann1 = IniIntGet(L"Sound", L"here1", 21, detailswitcher_ini_path);
	g_trigger.ini_ann2 = IniIntGet(L"Sound", L"here2", 22, detailswitcher_ini_path);
	//g_trigger.ini_ann3 = IniIntGet(L"Sound", L"here3", 23, detailswitcher_ini_path);
	g_trigger.ini_com1 = IniIntGet(L"Panel", L"First", 249, detailswitcher_ini_path);
	g_trigger.ini_com2 = IniIntGet(L"Panel", L"Second", 250, detailswitcher_ini_path);
	//g_trigger.ini_com3 = IniIntGet(L"Panel", L"Third", 251, detailswitcher_ini_path);
	g_trigger.ini_com = IniIntGet(L"Panel", L"Company", 252, detailswitcher_ini_path);
	g_trigger.ini_wait = IniIntGet(L"Trigger", L"Seconds", 30*1000, detailswitcher_ini_path);
	g_trigger.ini_so1 = IniIntGet(L"Comp1", L"1", 59, detailswitcher_ini_path);
	g_trigger.ini_so2 = IniIntGet(L"Comp1", L"2", 60, detailswitcher_ini_path);
	g_trigger.ini_so3 = IniIntGet(L"Comp1", L"3", 61, detailswitcher_ini_path);
	g_trigger.ini_so4 = IniIntGet(L"Comp1", L"4", 62, detailswitcher_ini_path);
	g_trigger.ini_so5 = IniIntGet(L"Comp1", L"5", 63, detailswitcher_ini_path);
	g_trigger.ini_jr1 = IniIntGet(L"Comp2", L"1", 59, detailswitcher_ini_path);
	g_trigger.ini_jr2 = IniIntGet(L"Comp2", L"2", 60, detailswitcher_ini_path);
	g_trigger.ini_jr3 = IniIntGet(L"Comp2", L"3", 61, detailswitcher_ini_path);
	g_trigger.ini_jr4 = IniIntGet(L"Comp2", L"4", 62, detailswitcher_ini_path);
	g_trigger.ini_jr5 = IniIntGet(L"Comp2", L"5", 63, detailswitcher_ini_path);
	g_trigger.ini_reset = IniIntGet(L"Setting", L"Reset", 1, detailswitcher_ini_path);
}

// Called when this plug-in is unloaded
void WINAPI atsDispose()
{
	for (int i = 0; i < g_num_of_detailmodules; ++i)
	{
		if (g_detailmodules[i].atsDispose != NULL)
		{
			g_detailmodules[i].atsDispose();
		}
		FreeLibrary(g_detailmodules[i].hm_dll);
	}

	memset(g_detailmodules, 0, sizeof(ST_DETAILMODULE_ATS_DELEGATE_FUNC));
	g_num_of_detailmodules = 0;
	memset(g_handles, 0, sizeof(ATS_HANDLES));
}

// Returns the version numbers of ATS plug-in
int WINAPI atsGetPluginVersion()
{
	return ATS_VERSION;
}

// Called when the train is loaded
void WINAPI atsSetVehicleSpec(ATS_VEHICLESPEC vspec)
{
	for (int i = 0; i < g_num_of_detailmodules; ++i)
	{
		if (g_detailmodules[i].atsSetVehicleSpec != NULL)
		{
			g_detailmodules[i].atsSetVehicleSpec(vspec);
		}
	}
}

// Called when the game is started
void WINAPI atsInitialize(int param)
{
	for (int i = 0; i < g_num_of_detailmodules; ++i)
	{
		if (g_detailmodules[i].atsInitialize != NULL)
		{
			g_detailmodules[i].atsInitialize(param);
		}
	}

	g_trigger.reset();
}

// Called every frame
ATS_HANDLES WINAPI atsElapse(ATS_VEHICLESTATE vs, int* p_panel, int* p_sound)
{
	if (g_load_failed)
	{
		char* p = 0; *p = 1;
	}
	
	ATS_HANDLES ret;

	g_trigger.set_distance(vs.Location);
	g_trigger.set_time(vs.Time);

	if (g_num_of_detailmodules > 0)
	{
		if (g_first_time)
		{
			for (int i = 0; i < g_num_of_detailmodules; ++i)
			{
				if (g_detailmodules[i].atsSetPower != NULL)
				{
					g_detailmodules[i].atsSetPower(g_handles[0].Power);
				}

				if (g_detailmodules[i].atsSetBrake != NULL)
				{
					g_detailmodules[i].atsSetBrake(g_handles[0].Brake);
				}

				if (g_detailmodules[i].atsSetReverser != NULL)
				{
					g_detailmodules[i].atsSetReverser(g_handles[0].Reverser);
				}
			}

			g_first_time = false;
		}
		else
		{
			if (g_trigger.ini_reset != 0)
			{
				for (int i = 0; i < 256; ++i)
				{
					p_panel[i] = 0;
				}
				for (int i = 0; i < 256; ++i)
				{
					p_sound[i] = ATS_SOUND_STOP;
				}
			}

			if (g_handles[0].Power != g_handles[1].Power)
			{
				if (g_detailmodules[0].atsSetPower != NULL)
				{
					g_detailmodules[0].atsSetPower(g_handles[0].Power);
				}
			}

			if (g_handles[0].Brake != g_handles[1].Brake)
			{
				if (g_detailmodules[0].atsSetBrake != NULL)
				{
					g_detailmodules[0].atsSetBrake(g_handles[0].Brake);
				}
			}

			if (g_handles[0].Reverser != g_handles[1].Reverser)
			{
				if (g_detailmodules[0].atsSetReverser != NULL)
				{
					g_detailmodules[0].atsSetReverser(g_handles[0].Reverser);
				}
			}
		}

		g_handles[1] = g_handles[0];

		if (g_detailmodules[0].atsElapse != NULL)
		{
			ret = g_detailmodules[0].atsElapse(vs, p_panel, p_sound);
		}

		for (int i = 1; i < g_num_of_detailmodules; ++i)
		{
			if (!g_trigger.is_enable(i))
			{
				continue;
			}

			if (g_detailmodules[i].last_handle.Power != ret.Power)
			{
				if (g_detailmodules[i].atsSetPower != NULL)
				{
					g_detailmodules[i].atsSetPower(ret.Power);
				}
			}

			if (g_detailmodules[i].last_handle.Brake != ret.Brake)
			{
				if (g_detailmodules[i].atsSetBrake != NULL)
				{
					g_detailmodules[i].atsSetBrake(ret.Brake);
				}
			}

			if (g_detailmodules[i].last_handle.Reverser != ret.Reverser)
			{
				if (g_detailmodules[i].atsSetReverser != NULL)
				{
					g_detailmodules[i].atsSetReverser(ret.Reverser);
				}
			}

			g_detailmodules[i].last_handle = ret;

			if (g_detailmodules[i].atsElapse != NULL)
			{
				ret = g_detailmodules[i].atsElapse(vs, p_panel, p_sound);
			}
		}
	}
	else
	{
		memset(&ret, 0, sizeof(ATS_HANDLES));
	}

	g_trigger.output_company(p_panel);
	g_trigger.announce_here(p_sound);

	return ret;
}

// Called when the power is changed
void WINAPI atsSetPower(int notch)
{
	g_handles[0].Power = notch;
}

// Called when the brake is changed
void WINAPI atsSetBrake(int notch)
{
	g_handles[0].Brake = notch;
}

// Called when the reverser is changed
void WINAPI atsSetReverser(int pos)
{
	g_handles[0].Reverser = pos;
}

// Called when any ATS key is pressed
void WINAPI atsKeyDown(int ats_key_code)
{
	for (int i = 0; i < g_num_of_detailmodules; ++i)
	{
		if (!g_trigger.is_enable(i))
		{
			continue;
		}

		if (g_detailmodules[i].atsKeyDown != NULL)
		{
			g_detailmodules[i].atsKeyDown(ats_key_code);
		}
	}
}

// Called when any ATS key is released
void WINAPI atsKeyUp(int ats_key_code)
{
	for (int i = 0; i < g_num_of_detailmodules; ++i)
	{
		if (!g_trigger.is_enable(i))
		{
			continue;
		}

		if (g_detailmodules[i].atsKeyUp != NULL)
		{
			g_detailmodules[i].atsKeyUp(ats_key_code);
		}
	}
}

// Called when the horn is used
void WINAPI atsHornBlow(int ats_horn)
{
	for (int i = 0; i < g_num_of_detailmodules; ++i)
	{
		if (!g_trigger.is_enable(i))
		{
			continue;
		}

		if (g_detailmodules[i].atsHornBlow != NULL)
		{
			g_detailmodules[i].atsHornBlow(ats_horn);
		}
	}
}

// Called when the door is opened
void WINAPI atsDoorOpen()
{
	g_trigger.set_door_open();

	for (int i = 0; i < g_num_of_detailmodules; ++i)
	{
		if (!g_trigger.is_enable(i))
		{
			continue;
		}

		if (g_detailmodules[i].atsDoorOpen != NULL)
		{
			g_detailmodules[i].atsDoorOpen();
		}
	}
}

// Called when the door is closed
void WINAPI atsDoorClose()
{
	g_trigger.set_door_close();

	for (int i = 0; i < g_num_of_detailmodules; ++i)
	{
		if (!g_trigger.is_enable(i))
		{
			continue;
		}

		if (g_detailmodules[i].atsDoorClose != NULL)
		{
			g_detailmodules[i].atsDoorClose();
		}
	}
}

// Called when current signal is changed
void WINAPI atsSetSignal(int signal)
{
	for (int i = 0; i < g_num_of_detailmodules; ++i)
	{
		/*
		if (!g_trigger.is_enable(i))
		{
			continue;
		}
		*/
		if (g_detailmodules[i].atsSetSignal != NULL)
		{
			g_detailmodules[i].atsSetSignal(signal);
		}
	}
}

// Called when the beacon data is received
void WINAPI atsSetBeaconData(ATS_BEACONDATA beacon_data)
{
	for (int i = 0; i < g_num_of_detailmodules; ++i)
	{
		/*
		if (!g_trigger.is_enable(i))
		{
			continue;
		}
		*/
		if (g_detailmodules[i].atsSetBeaconData != NULL)
		{
			g_detailmodules[i].atsSetBeaconData(beacon_data);
		}
	}
	if (beacon_data.Type == 601) {
		g_trigger.change_distance(beacon_data.Optional);
	}
	if (beacon_data.Type == 602) {
		g_trigger.sotetsu_rinkai(beacon_data.Optional);
	}
}

//UINT WINAPI GetPrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName)