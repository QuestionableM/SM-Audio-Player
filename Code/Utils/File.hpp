#pragma once

#include <shobjidl.h>
#include <string>
#include <vector>

namespace File
{
	bool ReadToString(const std::wstring& path, std::string& r_output);

	std::wstring OpenFileDialog(
		const std::wstring& title,
		FILEOPENDIALOGOPTIONS options = 0,
		std::vector<COMDLG_FILTERSPEC> filters = {},
		HWND owner = nullptr
	);

	std::wstring SaveFileDialog(
		const std::wstring& title,
		FILEOPENDIALOGOPTIONS options = 0,
		std::vector<COMDLG_FILTERSPEC> filters = {},
		std::vector<std::wstring> filter_ext = {},
		HWND owner = nullptr
	);
}