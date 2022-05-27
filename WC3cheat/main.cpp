#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <Psapi.h>
#include <format>
#include <cassert>
#include <memory>
#include <string>
#include <stdexcept>

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

struct Building {
    float hp, hpRegen, unknown, maxHp;
};

void OutputDebugString(const char msg[]) {
    std::cout << msg << std::endl;
}


struct Unit {
    float hp, hpRegen, unknown, maxHp;
};

DWORD address = 0x1F8F7B94;

void printError(const char additional[]) {
    std::cout << GetLastError() << " <-- error:" << additional << std::endl;
    exit(-1);
}

void testRead(HANDLE pHandle) {

    Unit unit;


    float hp;

    if (ReadProcessMemory(pHandle, (void* )address, &unit, sizeof(Unit), 0)) {


        std::cout << unit.hp << " " << unit.hpRegen << " " << unit.unknown << " " << unit.maxHp << " " << std::endl;
    }
    else {
        printError("Read from memory");
    }
}


int main() {

    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_READ, FALSE, 18120);
    
    if (!pHandle) {
        printError("Open process");
    }




    float newHP = 333;

    SIZE_T bytesWritten;

    if (WriteProcessMemory(pHandle, (void*)address, &newHP, sizeof(newHP), &bytesWritten)) {
        std::cout << "Written to memory" << std::endl;
        testRead(pHandle);
    }
    else {
        printError("Memory write");
    }
}
