#pragma once

#include "windows_include.hpp"
#include <string>

namespace Process
{
	HMODULE GetBaseAddress(HANDLE process, std::string& proc_name);
	bool GetProcessName(HANDLE process, std::string& output_string);
	bool IsRunning(HANDLE process);
	char IsWow64(HANDLE process);
}