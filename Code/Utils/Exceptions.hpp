#pragma once

#include "windows_include.hpp"
#include <string>

std::string GetErrorString(DWORD message_id);
std::string GetLastErrorString();

class GraphicsException
{
	std::string m_error_msg;
	std::string	m_expression;

public:
	GraphicsException(HRESULT hr, const char* expr_str);

	inline std::string GetString() const
	{
		return "Line: " + m_expression + "\n\nError: " + m_error_msg;
	}
};

#define GFX_TEST_THROW(hrcall) if (FAILED(hr = (hrcall))) throw GraphicsException(hr, #hrcall)