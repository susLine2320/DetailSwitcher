#include"source\ini.h"   // #include "Ats.h"�̏�ɏ���

void IniLoad(void)
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
	loadCheck = ini.load(fullpath);
	//INI�t�@�C���̃��[�h�Ɏ��s�����ꍇ
	/*
	if (loadCheck == false) {
		//���[�h���s
		return false;
	}
	//���[�h����
	return true;
	*/
}