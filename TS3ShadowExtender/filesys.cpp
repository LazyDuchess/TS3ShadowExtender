#include "pch.h"
#include "filesys.h"

namespace FileSys
{
    bool Exists(const std::wstring& name) {
        struct _stat buffer;
        return (_wstat(name.c_str(), &buffer) == 0);
    }

    std::wstring GetModuleDirectory()
    {
        wchar_t modName[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, modName, MAX_PATH);
        std::wstring::size_type pos = std::wstring(modName).find_last_of(L"\\/");
        auto folder = std::wstring(modName).substr(0, pos);
        return folder;
    }

    std::wstring GetAbsolutePathAuto(std::wstring path)
    {
        if (Exists(path))
            return path;
        std::wstring directory = GetModuleDirectory();
        wchar_t modName[MAX_PATH] = { 0 };
        wcscpy_s(modName, directory.c_str());
        wcscat_s(modName, L"\\");
        wcscat_s(modName, path.c_str());
        if (Exists(modName))
            return modName;
        return path;
    }
}