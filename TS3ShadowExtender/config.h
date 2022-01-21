#pragma once
#include <map>
#include <string>

typedef std::map<std::wstring, std::wstring> KeyMap;

class ConfigObject {
public:
	ConfigObject(std::wstring file, bool &result);
	std::wstring GetWString(std::wstring key, std::wstring defaultValue);
	float GetFloat(std::wstring key, float defaultValue);
	bool GetBool(std::wstring key, bool defaultValue);
	int GetInt(std::wstring key, int defaultValue);
	void SetKey(std::wstring key, std::wstring value);
	bool HasKey(std::wstring key);
private:
	KeyMap mKeys;
};