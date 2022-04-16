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
uintptr_t emptyTextPath = 0;
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
int(__cdecl* TickerFunc1)(void* a1, int a2) = nullptr;
int __cdecl TickerFunc1_rep(void* a1, int a2)
{
    //std::cout << "Fired1: " << (char*)a1 << std::endl;
    TickerOut((char*)a1);

    return TickerFunc1(a1, a2);
}

int(__cdecl* TickerFunc2)(void* a1, int a2) = nullptr;
int __cdecl TickerFunc2_rep(void* a1, int a2)
{
    //std::cout << "Fired2: " << (char*)a1 << std::endl;
    TickerOut((char*)a1);

    return TickerFunc2(a1, a2);
}

int(__cdecl* TickerFunc3)(void* a1) = nullptr;
int __cdecl TickerFunc3_rep(void* a1)
{
    if ((uintptr_t)a1 != emptyTextPath)
    {
        //std::cout << "Fired3: " << (char*)a1 << (int*)a1 << std::endl;
        TickerOut((char*)a1);
    }
    return TickerFunc3(a1);
}

// Separate thread for the server
DWORD WINAPI StartServer(LPVOID dll_instance)
{
    std::cout << "Server started!" << std::endl;
    s.run(10573);

    return EXIT_SUCCESS;
}

// DllMain
BOOL APIENTRY DllMain(HMODULE dll_instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH)
    {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        DisableThreadLibraryCalls(dll_instance);

        MH_Initialize();

        bm2dx_addr = uintptr_t(GetModuleHandleA("bm2dx.dll"));
        emptyTextPath = bm2dx_addr + 0x133000;

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

        uintptr_t TickerFunc1Addr = 0x896C0;
        uintptr_t TickerFunc2Addr = 0x89650;
        uintptr_t TickerFunc3Addr = 0x89860;

        MH_CreateHook(
            reinterpret_cast<void*>(bm2dx_addr + TickerFunc1Addr),
            reinterpret_cast<void*>(TickerFunc1_rep),
            reinterpret_cast<void**>(&TickerFunc1)
        );
        MH_CreateHook(
            reinterpret_cast<void*>(bm2dx_addr + TickerFunc2Addr),
            reinterpret_cast<void*>(TickerFunc2_rep),
            reinterpret_cast<void**>(&TickerFunc2)
        );
        MH_CreateHook(
            reinterpret_cast<void*>(bm2dx_addr + TickerFunc3Addr),
            reinterpret_cast<void*>(TickerFunc3_rep),
            reinterpret_cast<void**>(&TickerFunc3)
        );

        MH_EnableHook(MH_ALL_HOOKS);

        std::cout << "TickerHook for IIDX20 (2013090900)" << std::endl;
        std::cout << "Base module address is: ";
        std::cout << std::hex << bm2dx_addr << std::endl;

        if (reason == DLL_PROCESS_DETACH)
            MH_Uninitialize();
    }
    return TRUE;
}