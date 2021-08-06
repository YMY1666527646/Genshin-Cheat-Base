#include <pch.h>
#include <thread>

#include "../pattern scanner/Memory Pattern Scanner.h"
#include "../pattern scanner/Patterns List.h"
#include "../pattern scanner/Pattern Scanner.h"
#include "../configs/Configs.h"

int main()
{
	//Not used, just put "cfg.Unlock_FPS" to checkbox :)

	Config cfg;

	Memory_Pattern_Scanner Pattern_FPS(fpsFuncPattern, fpsFuncMask, sizeof(fpsFuncPattern) / sizeof(BYTE));

	MODULEENTRY32 UnityPlayer{};

	DWORD pID = NULL;

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);

	DWORD64 dwFpsFuncAddress = (DWORD64)ExtScan(Pattern_FPS, UnityPlayer.modBaseAddr, (uintptr_t)UnityPlayer.modBaseSize, hProcess);
	DWORD64 dwLimiterAddress = NULL;

	if (!ReadProcessMemory(hProcess, (LPCVOID)(dwFpsFuncAddress + 4), &dwLimiterAddress, sizeof(__int32), 0))
		std::cout << GetLastError() << std::endl;

	dwLimiterAddress += dwFpsFuncAddress + 0x8;

	__int32 targetFPS = cfg.Unlock_FPS;

	DWORD dwExitCode = STILL_ACTIVE;

	while (dwExitCode == STILL_ACTIVE) 
	{
		__int32 currentFPS = NULL;
		GetExitCodeProcess(hProcess, &dwExitCode);
		std::this_thread::sleep_for(std::chrono::seconds(8));

		ReadProcessMemory(hProcess, (LPCVOID)dwLimiterAddress, &currentFPS, sizeof(__int32), 0);
		if (currentFPS == -1)
			continue;
		if (currentFPS != targetFPS)
			WriteProcessMemory(hProcess, (LPVOID)dwLimiterAddress, &targetFPS, sizeof(__int32), 0);
	}
}