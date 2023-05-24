#include "Process.hpp"

#include <Psapi.h>

HMODULE Process::GetBaseAddress(HANDLE process, std::string& proc_name)
{
	HMODULE hMods[1024];
	DWORD cbNeeded;

	if (!EnumProcessModules(process, hMods, sizeof(hMods), &cbNeeded))
		return nullptr;

	for (unsigned int a = 0; a < (cbNeeded / sizeof(HMODULE)); a++)
	{
		CHAR szModName[MAX_PATH];
		if (GetModuleFileNameExA(process, hMods[a], szModName, sizeof(szModName)))
		{
			const std::string v_proc_name_str = szModName;

			if (v_proc_name_str.find(proc_name) != std::string::npos)
				return hMods[a];
		}
	}

	return nullptr;
}

bool Process::GetProcessName(HANDLE process, std::string& output_string)
{
	CHAR proc_name_str[MAX_PATH];

	if (GetProcessImageFileNameA(process, proc_name_str, sizeof(proc_name_str) / sizeof(CHAR)) == 0)
		return false;

	output_string = proc_name_str;
	return true;
}

bool Process::IsRunning(HANDLE process)
{
	DWORD v_exit_code;

	if (GetExitCodeProcess(process, &v_exit_code) == 0)
		return false;

	return v_exit_code == STILL_ACTIVE;
}

char Process::IsWow64(HANDLE process)
{
	typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (fnIsWow64Process == NULL)
		return -1;

	BOOL is_wow64;
	if (!fnIsWow64Process(process, &is_wow64))
		return -1;

	return is_wow64;
}