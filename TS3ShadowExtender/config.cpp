#include "pch.h"
#include "config.h"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "stringhelper.h"
#include "filesys.h"

ConfigObject::ConfigObject(std::wstring file, bool &result)
{
    if (!FileSys::Exists(file))
    {
        result = false;
        return;
    }
    std::wifstream streamFile(file);
    std::wstring str;
    while (std::getline(streamFile, str))
    {
        str = TrimString(str, false);
        //str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
        if (wcslen(str.c_str()) > 0)
        {
            bool skip = false;
            if (wcscmp(str.substr(0, 1).c_str(), L"#") == 0)
                skip = true;
            if (wcslen(str.c_str()) > 1)
            {
                if (wcscmp(str.substr(0, 1).c_str(), L"/") == 0 && wcscmp(str.substr(1, 1).c_str(), L"/") == 0)
                    skip = true;
            }
            if (!skip)
            {
                std::wstring temp;
                std::vector<std::wstring> split;
                std::wstringstream wss(str);
                while (std::getline(wss, temp, L'='))
                {
                    split.push_back(temp);
                }
                SetKey(TrimString(split[0],false), TrimString(split[1],true));
            }
        }
    }
    streamFile.close();
    result = true;
}

void ConfigObject::SetKey(std::wstring key, std::wstring value)
{
    mKeys[key] = value;
}

std::wstring ConfigObject::GetWString(std::wstring key, std::wstring defaultValue)
{
    auto it = mKeys.find(key);
    if (it != mKeys.end())
    {
        return it->second;
    }
    return defaultValue;
}

bool ConfigObject::GetBool(std::wstring key, bool defaultValue)
{
    std::wstring value = ToLowerString(GetWString(key,L""));
    if (wcscmp(value.c_str(), L"") == 0)
        return defaultValue;
    if (wcscmp(value.c_str(),L"true") == 0)
        return true;
    if (wcscmp(value.c_str(), L"1") == 0)
        return true;
    return false;
}

float ConfigObject::GetFloat(std::wstring key, float defaultValue)
{
    std::wstring value = GetWString(key,L"");
    if (wcscmp(value.c_str(), L"") == 0)
        return defaultValue;
    return std::stof(value);
}

int ConfigObject::GetInt(std::wstring key, int defaultValue)
{
    std::wstring value = GetWString(key,L"");
    if (wcscmp(value.c_str(), L"") == 0)
        return defaultValue;
    return std::stoi(value);
}

bool ConfigObject::HasKey(std::wstring key)
{
    auto it = mKeys.find(key);
    if (it != mKeys.end())
        return true;
}