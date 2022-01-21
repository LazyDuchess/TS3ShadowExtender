#pragma once
#include <string>

namespace FileSys
{
	std::wstring GetModuleDirectory();
	std::wstring GetAbsolutePathAuto(std::wstring path);
	bool Exists(const std::wstring& name);
}