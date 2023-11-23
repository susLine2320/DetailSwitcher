// Ats.cpp : DLL アプリケーション用のエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "atsplugin.h"
#include "ini.h"

DetailSwitcherIni ini;

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	//検索文字列へのポインタ
	char* posIni;
	char filePath[_MAX_PATH + 1] = _T("");
	//char filePath[MAX_PATH + 1] = _T("");
	bool loadCheck; // INIファイルのロードに成功したかどうか
	//Ats.dllのファイルパスを取得
	::GetModuleFileName((HMODULE)hinstDLL, filePath, MAX_PATH);
	//パスから.dllの位置を検索
	posIni = strstr(filePath, ".dll");
	//posIni = strstr(fullpath,".dll");
	//.dllを.iniに置換
	memmove(posIni, ".ini", 4);
	// INIファイルをロードして結果を取得
	loadCheck = ini.load(filePath);
	//INIファイルのロードに失敗した場合
	/*
	if (loadCheck == false) {
		//ロード失敗
		return false;
	}
	//ロード成功
	return true;
	*/
	return TRUE;
}
/*
ATS_API int WINAPI GetPluginVersion()
{
	return ATS_VERSION;
}

ATS_API void WINAPI SetVehicleSpec(ATS_VEHICLESPEC vehicleSpec)
{
	g_svcBrake = vehicleSpec.BrakeNotches;
	g_emgBrake = g_svcBrake + 1;
}

ATS_API void WINAPI Initialize(int brake)
{
	g_speed = 0;
}

ATS_API ATS_HANDLES WINAPI Elapse(ATS_VEHICLESTATE vehicleState, int* panel, int* sound)
{
	g_deltaT = vehicleState.Time - g_time;
	g_time = vehicleState.Time;
	g_speed = vehicleState.Speed;

	// ハンドル出力
	if (g_brakeNotch == g_emgBrake)
	{
		g_output.Brake = g_emgBrake;
	}
	else
	{
		g_output.Brake = g_brakeNotch;
	}
	if (g_pilotlamp == true)
	{
		g_output.Reverser = g_reverser;
	}
	else
	{
		g_output.Reverser = 0;
	}
	g_output.Power = g_powerNotch;
	g_output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;

	// パネル出力

	// サウンド出力

	return g_output;
}

ATS_API void WINAPI SetPower(int notch)
{
	g_powerNotch = notch;
}

ATS_API void WINAPI SetBrake(int notch)
{
	g_brakeNotch = notch;
}

ATS_API void WINAPI SetReverser(int pos)
{
	g_reverser = pos;
}

ATS_API void WINAPI KeyDown(int atsKeyCode)
{
}

ATS_API void WINAPI KeyUp(int hornType)
{
}

ATS_API void WINAPI HornBlow(int atsHornBlowIndex)
{
}

ATS_API void WINAPI DoorOpen()
{
	g_pilotlamp = false;
}

ATS_API void WINAPI DoorClose()
{
	g_pilotlamp = true;
}

ATS_API void WINAPI SetSignal(int signal)
{
}

ATS_API void WINAPI SetBeaconData(ATS_BEACONDATA beaconData)
{
}

ATS_API void WINAPI Load() {}
ATS_API void WINAPI Dispose() {}
*/