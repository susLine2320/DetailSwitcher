//
// Generated on 2023/02/21 by inimoni 
//

/*
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Documentation
  Struct = Section name, Field = Key name.
  The function used is only a declaration, load(), and save().
  The constructor automatically reads initialization file [initFileName].
  Please rewrite [initFileName] in the source freely.
  It becomes Windows folder when there is not path. The relative path can be used.

Example
#include "SampleIni.h"
void CTestDlg::OnButton1()
{
    //initFileName = "./sample.ini";
    SampleIni data1;                   // Open ./sample.ini
    int v = data1.Sample.value;        // [Sample]-value Get
    data1.Sample.value = v+1;          // [Sample]-value Modify
    data1.save();                      // Save ./Sample.ini (Opened file)

    SampleIni data2("sample.ini");     // Open C:/WINDOWS/Sample.ini
    data2.load("./sample.ini");        // Open Specified file.
    data2.save();                      // Save Opened file (Making at not being)
}

Supplementation
  Reading and writing becomes the unit of the file.
  [initFileName] is the same as the argument of GetPrivateProfileString() of SDK.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

#ifndef DETAILSWITCHERINI_H
#define DETAILSWITCHERINI_H

#include <string>
using namespace std;

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#ifdef _UNICODE
typedef std::wstring string_t;
#else
typedef std::string string_t;
#endif

namespace inimoni {
    template<class T>
    static bool inirw(int is_read, string_t& fname, string_t sec, string_t key, T& val_t);
};

/**
    DetailSwitcher.ini input output class.
*/
class DetailSwitcherIni
{
public:

    DetailSwitcherIni(string_t fname = _T(""))
    {
        initFileName = _T("./DetailSwitcher.ini"); //Default file.    
        init();
        load(fname);
    }

    /**
        INI file is read.
        @param fname Filename
        When there is not a file, It becomes initFileName.
        When there is not a pass, It becomes Windows folder.
    */
    bool load(string_t fname = _T(""))
    {
        if (fname.empty()) {
            fname = initFileName;
        }
        loadFileName = fname;
        WIN32_FIND_DATA fd;
        HANDLE h = ::FindFirstFile(fname.c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            iniRW(fname, 1);
        }
        return (h != INVALID_HANDLE_VALUE);
    }

    /**
        It writes it in the INI file.
        @param fname Filename
        When there is not a file, It becomes open file.
        When there is not a pass, It becomes Windows folder.
    */
    bool save(string_t fname = _T(""))
    {
        if (fname.empty()) {
            fname = loadFileName;
        }
        iniRW(fname, 0);
        return true;
    }

public:

    // Trigger
    struct _Trigger
    {
        int      Distance;                //この距離を超えたら切替[m]（不使用）
        int      Seconds;                 //開扉からこの時間で切替判定[s]
    } Trigger;

    // Panel
    struct _Panel
    {
        int      First;                   //1個目の事業者表示
        int      Second;                  //2個目の事業者表示
        int      Company;                 //事業者表示
    } Panel;

    // Sound
    struct _Sound
    {
        int      here1;                   //2個目に切り替えた時の音
        int      here2;                   //1個目に切り替えた時の音
    } Sound;

    // Details
    struct _Details
    {
        string_t PI1;                     //1個目のみ有効にするPI
        string_t PI2;                     //2個目のみ有効にするPI
    } Details;

protected:

    string_t initFileName;
    string_t loadFileName;

    bool iniRW(string_t f, int r)
    {
        string_t s;

        s = _T("Trigger");
        inimoni::inirw(r, f, s, _T("Distance         "), Trigger.Distance);
        inimoni::inirw(r, f, s, _T("Seconds          "), Trigger.Seconds);

        s = _T("Panel");
        inimoni::inirw(r, f, s, _T("First            "), Panel.First);
        inimoni::inirw(r, f, s, _T("Second           "), Panel.Second);
        inimoni::inirw(r, f, s, _T("Company          "), Panel.Company);

        s = _T("Sound");
        inimoni::inirw(r, f, s, _T("here1            "), Sound.here1);
        inimoni::inirw(r, f, s, _T("here2            "), Sound.here2);

        s = _T("Details");
        inimoni::inirw(r, f, s, _T("PI1              "), Details.PI1);
        inimoni::inirw(r, f, s, _T("PI2              "), Details.PI2);
        return true;
    }

    void init()
    {
        Trigger.Distance = 2100;
        Trigger.Seconds = 30;
        Panel.First = 244;
        Panel.Second = 245;
        Panel.Company = 246;
        Sound.here1 = 21;
        Sound.here2 = 22;
        Details.PI1 = _T("4,5");
        Details.PI2 = _T("2,5,6");
    }
};
typedef DetailSwitcherIni DetailSwitcherFile; //�V���݊�

//---------------------------------------------------------------------------
// Common method                                                             
//---------------------------------------------------------------------------
#ifndef INIMONI_INIRW                                                        
#define INIMONI_INIRW                                                        
namespace inimoni
{
    /*
    Read and Write INI file
      int     is_read  1=Read mode, 0=Write mode
      string  fname    Filename (The Windows folder when there is not path)
      string  sec      Section name
      string  key      Key name
      T       val_t    [Read]Init+Output, [Write]Input
    */
    template<class T>
    bool inirw(int is_read, string_t& fname, string_t sec, string_t key, T& val_t)
    {
        if (is_read) {
            inimoni::read(fname.c_str(), sec.c_str(), key.c_str(), val_t);
        }
        else {
            inimoni::write(fname.c_str(), sec.c_str(), key.c_str(), val_t);
        }
        return true;
    }

    bool read(string_t ifn, string_t sec, string_t key, int& dst)
    {
        dst = GetPrivateProfileInt(sec.c_str(), key.c_str(), dst, ifn.c_str());
        return true;
    }

    bool read(string_t ifn, string_t sec, string_t key, basic_string<TCHAR>& dst)
    {
        TCHAR buf[256];
        GetPrivateProfileString(
            sec.c_str(),
            key.c_str(),
            dst.c_str(),
            buf,
            sizeof(buf),
            ifn.c_str());
        dst = buf;
        return true;
    }

    bool read(string_t ifn, string_t sec, string_t key, double& dst)
    {
        string_t s;
        inimoni::read(ifn, sec, key, s);

        TCHAR* e;
        double x = _tcstod(s.c_str(), &e);

        dst = 0.0;
        if (!*e) {
            dst = x;
        }
        return true;
    }

    template<class T>
    bool write(string_t ifn, string_t sec, string_t key, T val_t)
    {
        TCHAR val[1024];
        inimoni::to_string(val, val_t);
        WritePrivateProfileString(sec.c_str(), key.c_str(), val, ifn.c_str());
        return true;
    }

    void to_string(TCHAR* str, int val)
    {
        _stprintf(str, _T("%d"), val);
    }

    void to_string(TCHAR* str, double val)
    {
        _stprintf(str, _T("%f"), val);
    }

    void to_string(TCHAR* str, basic_string<TCHAR> val)
    {
        _stprintf(str, _T("%s"), val.c_str());
    }
};

#endif                                                                  
#endif                                                                  

