// Ats.cpp : DLL �A�v���P�[�V�����p�̃G���g�� �|�C���g���`���܂��B
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
	//����������ւ̃|�C���^
	char* posIni;
	char filePath[_MAX_PATH + 1] = _T("");
	//char filePath[MAX_PATH + 1] = _T("");
	bool loadCheck; // INI�t�@�C���̃��[�h�ɐ����������ǂ���
	//Ats.dll�̃t�@�C���p�X���擾
	::GetModuleFileName((HMODULE)hinstDLL, filePath, MAX_PATH);
	//�p�X����.dll�̈ʒu������
	posIni = strstr(filePath, ".dll");
	//posIni = strstr(fullpath,".dll");
	//.dll��.ini�ɒu��
	memmove(posIni, ".ini", 4);
	// INI�t�@�C�������[�h���Č��ʂ��擾
	loadCheck = ini.load(filePath);
	//INI�t�@�C���̃��[�h�Ɏ��s�����ꍇ
	/*
	if (loadCheck == false) {
		//���[�h���s
		return false;
	}
	//���[�h����
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

	// �n���h���o��
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

	// �p�l���o��

	// �T�E���h�o��

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