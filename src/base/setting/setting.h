﻿
//How to use or test

//    SSetting setting("config.ini",SETTING_INI);
//    SDebug("key2 ==> %s",setting.getValue("group1","key2").c_str());
//    setting.setValue("group1","key2","22");
//    SDebug("key2 ==> %s",setting.getValue("group1","key2").c_str());

#ifndef SETTING_H
#define SETTING_H

#include <string>
#include <map>
using namespace std;

enum SettingType{SETTING_XML,SETTING_INI};

class SSetting
{
private:
    string fileName;
    SettingType settingType;
    map<string,map<string,string> > maps;
    bool changed;
private:
    void init();
    void save();
    void readIni();
    void readXml();
    void writeIni();
    void writeXml();
public:
    SSetting(const char *fname, SettingType type);
    ~SSetting();
    string getValue(const char* group,const char* key);
    void setValue(const char* group,const char* key,char* value);
    void print();
};

#endif // SETTING_H
