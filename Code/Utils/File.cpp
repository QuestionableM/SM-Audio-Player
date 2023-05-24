#include "File.hpp"

#include <ShlObj.h>
#include <fstream>

bool File::ReadToString(const std::wstring& path, std::string& r_output)
{
	std::ifstream input_file(path, std::ios::binary);
	if (!input_file.is_open()) return false;

	input_file.seekg(0, std::ios::end);
	r_output.resize(input_file.tellg());
	input_file.seekg(0, std::ios::beg);

	input_file.read(r_output.data(), r_output.size());
	input_file.close();

	return true;
}

std::wstring File::OpenFileDialog(
	const std::wstring& title,
	FILEOPENDIALOGOPTIONS options,
	std::vector<COMDLG_FILTERSPEC> filters,
	HWND owner)
{
	std::wstring v_out_path = L"";

	HRESULT hr = CoInitializeEx(NULL, COINIT_DISABLE_OLE1DDE | COINIT_APARTMENTTHREADED);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			pFileOpen->SetOptions(options);
			pFileOpen->SetTitle(title.c_str());
			pFileOpen->SetFileTypes(static_cast<UINT>(filters.size()), filters.data());
			hr = pFileOpen->Show(owner);

			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					if (SUCCEEDED(hr))
					{
						v_out_path = std::wstring(pszFilePath);
						CoTaskMemFree(pszFilePath);
					}

					pItem->Release();
				}
			}

			pFileOpen->Release();
		}

		CoUninitialize();
	}

	return v_out_path;
}

std::wstring File::SaveFileDialog(
	const std::wstring& title,
	FILEOPENDIALOGOPTIONS options,
	std::vector<COMDLG_FILTERSPEC> filters,
	std::vector<std::wstring> filter_ext,
	HWND owner)
{
	std::wstring v_out_path = L"";

	HRESULT hr = CoInitializeEx(NULL, COINIT_DISABLE_OLE1DDE | COINIT_APARTMENTTHREADED);
	if (SUCCEEDED(hr))
	{
		IFileSaveDialog* pFileSave;

		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
			IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr))
		{
			pFileSave->SetOptions(options);
			pFileSave->SetTitle(title.c_str());
			pFileSave->SetFileTypes(static_cast<UINT>(filters.size()), filters.data());
			hr = pFileSave->Show(owner);

			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileSave->GetResult(&pItem);

				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					if (SUCCEEDED(hr))
					{
						v_out_path = std::wstring(pszFilePath);

						UINT v_ext_idx = 0;
						hr = pFileSave->GetFileTypeIndex(&v_ext_idx);
						if (SUCCEEDED(hr))
						{
							const std::wstring& v_cur_ext_str = filter_ext[v_ext_idx - 1];
							if (v_out_path.find(v_cur_ext_str) == std::wstring::npos)
								v_out_path.append(v_cur_ext_str);
						}

						CoTaskMemFree(pszFilePath);
					}

					pItem->Release();
				}
			}

			pFileSave->Release();
		}

		CoUninitialize();
	}

	return v_out_path;
}