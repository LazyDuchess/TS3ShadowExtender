#include "pch.h"
#include "stringhelper.h"
#include <algorithm>

std::wstring ToLowerString(std::wstring str) {
	std::transform(
		str.begin(), str.end(),
		str.begin(),
		towlower);
	return str;
}

std::wstring TrimString(std::wstring str, bool removeQuotes) {
	int stringLength = wcslen(str.c_str());
	if (stringLength <= 0)
		return L"";
	wchar_t startString[MAX_PATH] = { 0 };
	bool trail = true;
	bool skip = true;
	int lastLetterIndex = -1;
	for (int i = 0; i < stringLength; i++)
	{
		if (trail)
			skip = true;
		else
			skip = false;
		if (wcscmp(str.substr(i, 1).c_str(), L" ") != 0)
		{
			lastLetterIndex = i;
			skip = false;
			trail = false;
		}
		if (!skip)
			wcscat_s(startString, str.substr(i, 1).c_str());
	}
	if (wcslen(startString) <= 0 || lastLetterIndex == -1)
		return L"";
	std::wstring finalString = std::wstring(startString).substr(0, lastLetterIndex + 1);
	int finalStringLength = wcslen(finalString.c_str());
	if (removeQuotes)
	{
		if (wcscmp(finalString.substr(0, 1).c_str(), L"\"") == 0 && wcscmp(finalString.substr(finalStringLength-1, 1).c_str(), L"\"") == 0)
		{
			return finalString.substr(1, finalStringLength - 2);
		}
	}
	return finalString;
}