#include"source\ini.h"   // #include "Ats.h"の上に書く

void IniLoad(void)
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
	loadCheck = ini.load(fullpath);
	//INIファイルのロードに失敗した場合
	/*
	if (loadCheck == false) {
		//ロード失敗
		return false;
	}
	//ロード成功
	return true;
	*/
}