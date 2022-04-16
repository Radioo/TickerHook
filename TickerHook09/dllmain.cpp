// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "WSServer.cpp"

#if _WIN64
#pragma comment(lib, "libMinHook.x64.lib")
#else
#pragma comment(lib, "libMinHook.x86.lib")
#endif

// Globals
std::map<std::string, std::string> params;
bool serverMode;
broadcast_server s;
uintptr_t bm2dx_addr = 0;
std::string FilePath;
void(*TickerOut)(const char*) = nullptr;

// Writes to specified file when file mode in on
void writeToFile(const char* text)
{
    std::ofstream tickerTextFile;
    tickerTextFile.open(FilePath);
    tickerTextFile << text;
    tickerTextFile.close();
}

// Sends text to the server when server mode is on
void sendViaServer(const char* text)
{
    s.send(text);
}

// Hooked functions
int(__cdecl* TickerFunc1) (const char* a1, int a2) = nullptr;
int __cdecl TickerFunc1_rep(const char* a1, int a2)
{
    //printf("TickerFunc1: %s\n", a1);

    TickerOut(a1);

    return TickerFunc1(a1, a2);
}

int(__cdecl* TickerFunc2)(char* a1, int a2) = nullptr;
int __cdecl TickerFunc2_rep(char* a1, int a2)
{
    //printf("TickerFunc2: %s\n", a1);

    TickerOut(a1);

    return TickerFunc2(a1, a2);
}

int(__cdecl* TickerFunc3)(char* a1) = nullptr;
int __cdecl TickerFunc3_rep(char* a1)
{
    //printf("TickerFunc3: %s\n", a1);

    TickerOut(a1);

    return TickerFunc3(a1);
}

int(__cdecl* TickerFunc4)(const char* a1, int a2) = nullptr;
int __cdecl TickerFunc4_rep(const char* a1, int a2)
{
    //printf("TickerFunc4: %s\n", a1);

    TickerOut(a1);

    return TickerFunc4(a1, a2);
}

int(__cdecl* TickerClear)() = nullptr;
int TickerClear_replace()
{
    //printf("Ticker clear\n");

    TickerOut(" ");

    return TickerClear();
}

// Separate thread for the server
DWORD WINAPI StartServer(LPVOID dll_instance)
{
    std::cout << "Server started!" << std::endl;
    s.run(10573);

    return EXIT_SUCCESS;
}

BOOL APIENTRY DllMain(HMODULE dll_instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH)
    {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        DisableThreadLibraryCalls(dll_instance);

        MH_Initialize();

        bm2dx_addr = uintptr_t(GetModuleHandleA("bm2dx.exe"));

        // Parse config file
        std::ifstream cFile("TickerHook.ini");
        if (cFile.is_open())
        {
            std::string line;
            while (getline(cFile, line)) {
                line.erase(std::remove_if(line.begin(), line.end(), isspace),
                    line.end());
                if (line[0] == '#' || line.empty())
                    continue;
                auto delimiterPos = line.find("=");
                auto name = line.substr(0, delimiterPos);
                auto value = line.substr(delimiterPos + 1);
                std::cout << name << "=" << value << '\n';
                params.insert(std::pair<std::string, std::string>(name, value));
            }
            // set globals from config
            if (params["mode"] == "server")
            {
                serverMode = true;
                std::cout << "Server mode" << std::endl;
                CreateThread(NULL, NULL, StartServer, dll_instance, NULL, NULL);
                TickerOut = &sendViaServer;
            }
            else
            {
                std::cout << "File mode" << std::endl;
                FilePath = params["filepath"];
                TickerOut = &writeToFile;
            }

        }
        else {
            serverMode = true;
            std::cout << "Server mode" << std::endl;
            CreateThread(NULL, NULL, StartServer, dll_instance, NULL, NULL);
            TickerOut = &sendViaServer;
        }

        uintptr_t TickerFuncOffset1 = 0x216F0;
        uintptr_t TickerFuncOffset2 = 0x21800;
        uintptr_t TickerFuncOffset3 = 0x218A0;
        uintptr_t TickerFuncOffset4 = 0x215B0;
        uintptr_t TickerFuncOffset5 = 0x21920;

        MH_CreateHook(
            reinterpret_cast<void*>(bm2dx_addr + TickerFuncOffset1),
            reinterpret_cast<void*>(TickerFunc1_rep),
            reinterpret_cast<void**>(&TickerFunc1)
        );
        MH_CreateHook(
            reinterpret_cast<void*>(bm2dx_addr + TickerFuncOffset2),
            reinterpret_cast<void*>(TickerFunc2_rep),
            reinterpret_cast<void**>(&TickerFunc2)
        );
        MH_CreateHook(
            reinterpret_cast<void*>(bm2dx_addr + TickerFuncOffset3),
            reinterpret_cast<void*>(TickerFunc3_rep),
            reinterpret_cast<void**>(&TickerFunc3)
        );
        MH_CreateHook(
            reinterpret_cast<void*>(bm2dx_addr + TickerFuncOffset4),
            reinterpret_cast<void*>(TickerFunc4_rep),
            reinterpret_cast<void**>(&TickerFunc4)
        );
        MH_CreateHook(
            reinterpret_cast<void*>(bm2dx_addr + TickerFuncOffset5),
            reinterpret_cast<void*>(TickerClear_replace),
            reinterpret_cast<void**>(&TickerClear)
        );
        MH_EnableHook(MH_ALL_HOOKS);

        std::cout << "TickerHook for IIDX09 (JAG)" << std::endl;
        std::cout << "Base module address is: ";
        std::cout << std::hex << bm2dx_addr << std::endl;

        if (reason == DLL_PROCESS_DETACH)
            MH_Uninitialize();
    }
    return TRUE;
}