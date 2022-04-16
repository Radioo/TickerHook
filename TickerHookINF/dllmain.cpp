// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "WSServer.cpp"

#if _WIN64
#pragma comment(lib, "libMinHook.x64.lib")
#else
#pragma comment(lib, "libMinHook.x86.lib")
#endif

// Globals
broadcast_server s;

struct musicInfo {
public:
    char* title;
    char* tickerText;
    char* genre;
    char* artist;
    musicInfo(uintptr_t& data)
    {
        title = (char*)data;
        tickerText = (char*)(data + 64);
        genre = (char*)(data + 128);
        artist = (char*)(data + 192);
    }
};

void TickerOut(uintptr_t text)
{
    musicInfo music(text);
    std::cout << "Artist: " << music.artist << " Title: " << music.title << " Genre: " << music.genre << " Ticker: "
        << music.tickerText << std::endl;
    s.send(music.tickerText);
}

#pragma region Hooks
void(__fastcall* TickerFunc1)(DWORD* a1, __int64 musicPtr) = nullptr;
std::vector<BYTE> TickerFunc1_vec = { 0x40, 0x56, 0x57, 0x41, 0x55, 0x48, 0x83, 0xEC,
0x20, 0x48, 0x8D, 0x79, 0x3C, 0x4C, 0x8B, 0xEA, 0x48, 0x8B, 0xF1, 0x48, 0x85, 0xD2 };
void __fastcall TickerFunc1_rep(DWORD* a1, __int64 musicPtr)
{
    TickerOut(musicPtr);

    TickerFunc1(a1, musicPtr);
}

char(__fastcall* TickerFunc2)(__int64 a1, __int64 musicPtr, int a3) = nullptr;
std::vector<BYTE> TickerFunc2_vec = { 0x40, 0x55, 0x57, 0x41, 0x56, 0x48, 0x81, 0xEC,
0x80, 0x02, 0x00, 0x00, 0x48, 0x8B, 0x05, 0xBD, 0xE0, 0x3D, 0x00, 0x48, 0x33, 0xC4,
0x48, 0x89, 0x84, 0x24, 0x70, 0x02, 0x00, 0x00, 0x4C, 0x8B, 0xF1, 0x48, 0x89, 0x51,
0x08, 0x44, 0x89, 0x01, 0x48, 0x8B, 0xEA, 0x33, 0xC9, 0x8B, 0xC1, 0x41, 0x89, 0x4E, 0x20 };
char __fastcall TickerFunc2_rep(__int64 a1, __int64 musicPtr, int a3)
{
    TickerOut(musicPtr);

    return TickerFunc2(a1, musicPtr, a3);
}
#pragma endregion

void StartServer()
{
    std::cout << "Server started!" << std::endl;
    s.run(10573);
}

int CreateHooks()
{
    int hooksMade = 0;

    MODULEINFO moduleInfo;
    HMODULE bm2dxModule = GetModuleHandleA("bm2dx.exe");
    GetModuleInformation(GetCurrentProcess(), bm2dxModule, &moduleInfo, sizeof(moduleInfo));

    std::cout << "Module base address: " << moduleInfo.lpBaseOfDll << std::endl
        << "Module size: " << moduleInfo.SizeOfImage << " bytes" << std::endl;

    BYTE* addrLow = static_cast<BYTE*>(moduleInfo.lpBaseOfDll);
    BYTE* addrHigh = addrLow + moduleInfo.SizeOfImage;

    MH_Initialize();

    // search for functions
    BYTE* result1 = std::search(addrLow, addrHigh, TickerFunc1_vec.begin(), TickerFunc1_vec.end());
    if (result1 != addrHigh)
    {
        hooksMade++;
        std::cout << "Found TikcerFunc1 at: " << std::hex << (uintptr_t)result1 << std::endl;
        MH_CreateHook(
            reinterpret_cast<void*>((uintptr_t)result1),
            reinterpret_cast<void*>(TickerFunc1_rep),
            reinterpret_cast<void**>(&TickerFunc1)
        );
    }

    BYTE* result2 = std::search(addrLow, addrHigh, TickerFunc2_vec.begin(), TickerFunc2_vec.end());
    if (result2 != addrHigh)
    {
        hooksMade++;
        std::cout << "Found TickerFunc2 at: " << std::hex << (uintptr_t)result2 << std::endl;
        MH_CreateHook(
            reinterpret_cast<void*>((uintptr_t)result2),
            reinterpret_cast<void*>(TickerFunc2_rep),
            reinterpret_cast<void**>(&TickerFunc2)
        );
    }

    MH_EnableHook(MH_ALL_HOOKS);
    return hooksMade;
}

DWORD WINAPI Ticker_init(LPVOID dll_instance)
{
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

    int hooks = CreateHooks();

    std::cout << "Created " << hooks << " hooks" << std::endl;

    StartServer();

    return EXIT_SUCCESS;
}

// DllMain
BOOL APIENTRY DllMain(HMODULE dll_instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(dll_instance);
        CreateThread(NULL, NULL, Ticker_init, dll_instance, NULL, NULL);        
    }
    return TRUE;
}