#include "pch.h"
#include "hooking.h"

namespace Hooking
{
    void Nop(BYTE* pAddress, DWORD dwLen)
    {
        DWORD dwOldProtect, dwBkup, dwRelAddr;

        // give the paged memory read/write permissions

        VirtualProtect(pAddress, dwLen, PAGE_EXECUTE_READWRITE, &dwOldProtect);

        for (DWORD x = 0x0; x < dwLen; x++) *(pAddress + x) = 0x90;

        // restore the paged memory permissions saved in dwOldProtect

        VirtualProtect(pAddress, dwLen, dwOldProtect, &dwBkup);

        return;

    }

    //Thank you, https://www.unknowncheats.me/forum/c-and-c-/67884-mid-function-hook-deal.html !!!
    void MakeJMP(BYTE* pAddress, DWORD dwJumpTo, DWORD dwLen)
    {
        DWORD dwOldProtect, dwBkup, dwRelAddr;

        // give the paged memory read/write permissions

        VirtualProtect(pAddress, dwLen, PAGE_EXECUTE_READWRITE, &dwOldProtect);

        // calculate the distance between our address and our target location
        // and subtract the 5bytes, which is the size of the jmp
        // (0xE9 0xAA 0xBB 0xCC 0xDD) = 5 bytes

        dwRelAddr = (DWORD)(dwJumpTo - (DWORD)pAddress) - 5;

        // overwrite the byte at pAddress with the jmp opcode (0xE9)

        *pAddress = 0xE9;

        // overwrite the next 4 bytes (which is the size of a DWORD)
        // with the dwRelAddr

        *((DWORD*)(pAddress + 0x1)) = dwRelAddr;

        // overwrite the remaining bytes with the NOP opcode (0x90)
        // NOP opcode = No OPeration

        for (DWORD x = 0x5; x < dwLen; x++) *(pAddress + x) = 0x90;

        // restore the paged memory permissions saved in dwOldProtect

        VirtualProtect(pAddress, dwLen, dwOldProtect, &dwBkup);

        return;

    }

    void WriteToMemory(DWORD addressToWrite, void* valueToWrite, int byteNum)
    {
        //used to change our file access type, stores the old
        //access type and restores it after memory is written
        unsigned long OldProtection;
        //give that address read and write permissions and store the old permissions at oldProtection
        VirtualProtect((LPVOID)(addressToWrite), byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);

        //write the memory into the program and overwrite previous value
        memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);

        //reset the permissions of the address back to oldProtection after writting memory
        VirtualProtect((LPVOID)(addressToWrite), byteNum, OldProtection, NULL);
    }

    bool memory_readable(void* ptr, size_t byteCount)
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(ptr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
            return false;

        if (mbi.State != MEM_COMMIT)
            return false;

        if (mbi.Protect == PAGE_NOACCESS || mbi.Protect == PAGE_EXECUTE)
            return false;

        // This checks that the start of memory block is in the same "region" as the
        // end. If it isn't you "simplify" the problem into checking that the rest of 
        // the memory is readable.
        size_t blockOffset = (size_t)((char*)ptr - (char*)mbi.AllocationBase);
        size_t blockBytesPostPtr = mbi.RegionSize - blockOffset;

        if (blockBytesPostPtr < byteCount)
            return memory_readable((char*)ptr + blockBytesPostPtr,
                byteCount - blockBytesPostPtr);

        return true;
    }

    char* ScanBasic(char* pattern, char* mask, char* begin, intptr_t size)
    {
        intptr_t patternLen = strlen(mask);

        for (int i = 0; i < size; i++)
        {
            bool found = true;
            for (int j = 0; j < patternLen; j++)
            {
                if (mask[j] != '?' && pattern[j] != *(char*)((intptr_t)begin + i + j))
                {
                    found = false;
                    break;
                }
            }
            if (found)
            {
                return (begin + i);
            }
        }
        return nullptr;
    }

    char* ScanInternal(char* pattern, char* mask, char* begin, intptr_t size)
    {
        char* match{ nullptr };
        MEMORY_BASIC_INFORMATION mbi{};

        for (char* curr = begin; curr < begin + size; curr += mbi.RegionSize)
        {
            if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;

            match = ScanBasic(pattern, mask, curr, mbi.RegionSize);

            if (match != nullptr)
            {
                break;
            }
        }
        return match;
    }

    char* ScanInternalRetry(char* pattern, char* mask, char* begin, intptr_t size, int retries, int interval)
    {
        int currentTries = 1;
        char* addr = ScanInternal(pattern, mask, begin, size);
        while (addr == nullptr && currentTries < retries)
        {
            Sleep(interval);
            addr = ScanInternal(pattern, mask, begin, size);
            currentTries += 1;
        }
        return addr;
    }
}