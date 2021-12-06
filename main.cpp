/*
Written by DisabledMallis (https://github.com/DisabledMallis)

Originally made for the NKHook5 project, repurposed for Round8

Usage:
	This DLL is loaded when the game loads. This allows us to run our own code before the game's
	code so we can modify how it loads without patching the binary. This DLL searches for a relative
	directory called "loaders" which will contain any DLLs that should be injected before the game starts.
	These DLLs should be 'tweaker' DLLs that prepare & patch the game for mod loading.
*/
#include <Windows.h>
#include <string>
#include <iostream>
#include <shlobj_core.h>
#include <filesystem>
#include <cstdint>



//Used to ensure the functions required are properly exported
#define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

//HMODULE for the original dll
HMODULE winINet;
size_t(__stdcall* InternetGetConnectedState_orig)(size_t,int);

extern "C" __declspec(dllexport) size_t __stdcall InternetGetConnectedState(size_t lpdwFlags, int dwReserved) {
    int result = InternetGetConnectedState_orig(lpdwFlags, dwReserved);
    return result;
}

auto initialize() -> int {
	//Find the original wininet.dll
    char sys32Path[MAX_PATH];

	//We need to use diff dirs on diff platforms
	#if INTPTR_MAX == INT64_MAX
    SHGetFolderPathA(nullptr, CSIDL_SYSTEM, nullptr, SHGFP_TYPE_CURRENT, sys32Path);
	#elif INTPTR_MAX == INT32_MAX
    SHGetFolderPathA(nullptr, CSIDL_SYSTEMX86, nullptr, SHGFP_TYPE_CURRENT, sys32Path);
	#else
	#error Unknown pointer size or missing size macros!
	#endif

    std::string sys32Str(sys32Path);
    std::string wininetPath = sys32Str + "\\wininet.dll";

	
	//Load the original wininet dll
    winINet = LoadLibraryA(wininetPath.c_str());
	//Get the original function to call when requested
    InternetGetConnectedState_orig = (size_t(__stdcall*)(size_t,int))GetProcAddress(winINet, "InternetGetConnectedState");

    //Load tweaker DLLs
	std::string modsDir = "loaders/";
	for(const auto& tweaker : std::filesystem::directory_iterator(modsDir)) {
		LoadLibraryW(tweaker.path().c_str());
	}

    return 0;
}


extern "C" __declspec(dllexport) bool __stdcall DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
            initialize();
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}