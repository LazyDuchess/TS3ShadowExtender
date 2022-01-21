#include "ShadowExtender.h"
#include "pch.h"
#include <math.h>
#include <Psapi.h>
#include "hooking.h"
#include "config.h"
#include "filesys.h"

namespace ShadowExtender
{
	int retryAmount = 10;
	int retryInterval = 500;

	float shadowDistanceMax = 1200;
	float shadowDistanceMin = 150;
	float shadowDistanceMultiply = 2.0;
	bool mapViewShadows = true;

	char lookupTreeShadows[] = { 0x74, 0x49, 0xA1, 0x40, 0xF0, 0x1A, 0x01, 0x6A, 0x01, 0x68, 0x20, 0xAD, 0x0F, 0x01, 0x50, 0xE8, 0xE2, 0xD0, 0xC2, 0xFF, 0x83, 0xC4, 0x0C, 0x83 };
	char maskTreeShadows[] = "x?x????xxx????xx????xxxx";
	char jmpCode[] = { 0xEB };

	//replace 6 bytes, nop 1 after injected JMP
	char lookupShadowHook1[] = { 0xD9, 0xE8, 0x51, 0xD9, 0x1C, 0x24, 0x8B, 0xCE, 0xE8, 0xDE, 0xB5, 0x23, 0x00, 0xD9, 0x44, 0x24, 0x10, 0x51, 0x8B, 0xCE, 0xD9, 0x1C, 0x24, 0xE8 };
	char maskShadowHook1[] = "xxxxxxxxx????xxxxxxxxxxx";
	char* returnShadowHook1;

	//replace 5 bytes
	char lookupShadowHook2[] = { 0xD9, 0x44, 0x24, 0x08, 0x51, 0x8B, 0xCE, 0xD9, 0x1C, 0x24, 0xE8, 0x7B, 0xB7, 0x23, 0x00, 0xD9, 0xE8, 0x51, 0xD9, 0x1C, 0x24, 0x8B, 0xCE, 0xE8 };
	char maskShadowHook2[] = "xxxxxxxxxxx????xxxxxxxxx";
	char* returnShadowHook2;

	void _stdcall FixDistances(char* shadowStruct)
	{
		float* ptrXValue = (float*)(shadowStruct + 0x0C);
		float* ptrYValue = (float*)(shadowStruct + 0x10);
		ptrXValue[0] = fminf(fmaxf(ptrXValue[0] * shadowDistanceMultiply, shadowDistanceMin), shadowDistanceMax);
		ptrYValue[0] = fminf(fmaxf(ptrYValue[0] * shadowDistanceMultiply, shadowDistanceMin), shadowDistanceMax);
	}

	__declspec(naked) void shadowHook1()
	{
		__asm {
			push ecx
			call FixDistances
			fld1
			push ecx
			fstp dword ptr[esp]
			jmp returnShadowHook1
		}
	}

	__declspec(naked) void shadowHook2()
	{
		__asm {
			push ecx
			call FixDistances
			fld dword ptr[esp + 0x08]
			push ecx
			jmp returnShadowHook2
		}
	}

	void HookDistances() {
		HMODULE module = GetModuleHandleA(NULL);
		char* modBase = (char*)module;
		HANDLE proc = GetCurrentProcess();
		MODULEINFO modInfo;
		GetModuleInformation(proc, module, &modInfo, sizeof(MODULEINFO));
		int size = modInfo.SizeOfImage;
		DWORD addr = (DWORD)Hooking::ScanInternalRetry(lookupShadowHook1, maskShadowHook1, modBase, size, retryAmount, retryInterval);
		DWORD addr2 = (DWORD)Hooking::ScanInternalRetry(lookupShadowHook2, maskShadowHook2, modBase, size, retryAmount, retryInterval);

		if (addr != (DWORD)nullptr)
		{
			returnShadowHook1 = (char*)(addr + 6);
			Hooking::MakeJMP((BYTE*)addr, (DWORD)shadowHook1, 6);
		}

		if (addr2 != (DWORD)nullptr)
		{
			returnShadowHook2 = (char*)(addr2 + 5);
			Hooking::MakeJMP((BYTE*)addr2, (DWORD)shadowHook2, 5);
		}
	}

	void EnableTreeShadows() {
		HMODULE module = GetModuleHandleA(NULL);
		char* modBase = (char*)module;
		HANDLE proc = GetCurrentProcess();
		MODULEINFO modInfo;
		GetModuleInformation(proc, module, &modInfo, sizeof(MODULEINFO));
		int size = modInfo.SizeOfImage;
		DWORD addr = (DWORD)Hooking::ScanInternalRetry(lookupTreeShadows, maskTreeShadows, modBase, size, retryAmount, retryInterval);
		if (addr != (DWORD)nullptr)
		{
			//Replace JE with JMP to always enable tree shadows in map view.
			Hooking::WriteToMemory(addr, jmpCode, sizeof(jmpCode) / sizeof(*jmpCode));
		}
	}

	void Run() {
		bool cfgResult = false;

		ConfigObject config = ConfigObject(FileSys::GetAbsolutePathAuto(L"ShadowExtender.cfg"), cfgResult);

		if (cfgResult)
		{
			mapViewShadows = config.GetBool(L"TreeShadows", mapViewShadows);
			shadowDistanceMax = config.GetFloat(L"DistanceMax", shadowDistanceMax);
			shadowDistanceMin = config.GetFloat(L"DistanceMin", shadowDistanceMin);
			shadowDistanceMultiply = config.GetFloat(L"DistanceMultiply", shadowDistanceMultiply);
			retryAmount = config.GetInt(L"RetryAmount",retryAmount);
			retryInterval = config.GetInt(L"RetryInterval",retryInterval);
		}

		if (mapViewShadows)
			EnableTreeShadows();
		HookDistances();
	}
}