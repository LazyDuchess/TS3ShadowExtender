#pragma once
#include <string>

namespace Hooking
{
	void MakeJMP(BYTE* pAddress, DWORD dwJumpTo, DWORD dwLen);
	void Nop(BYTE* pAddress, DWORD dwLen);
	void WriteToMemory(DWORD addressToWrite, void* valueToWrite, int byteNum);
	bool memory_readable(void* ptr, size_t byteCount);
	char* ScanBasic(char* pattern, char* mask, char* begin, intptr_t size);
	char* ScanInternal(char* pattern, char* mask, char* begin, intptr_t size);
	char* ScanInternalRetry(char* pattern, char* mask, char* begin, intptr_t size, int retries, int interval);
}