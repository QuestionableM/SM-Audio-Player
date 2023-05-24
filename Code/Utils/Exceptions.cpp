#include "Exceptions.hpp"

std::string GetErrorString(DWORD message_id)
{
	if (message_id == 0)
		return "UNKNOWN_ERROR";

	LPSTR v_msgBuffer;
	const std::size_t v_msgSize = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		message_id,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		reinterpret_cast<LPSTR>(&v_msgBuffer),
		0,
		NULL
	);

	const std::string v_errorMsgStr(v_msgBuffer, v_msgSize);
	LocalFree(v_msgBuffer);

	return v_errorMsgStr;
}

std::string GetLastErrorString()
{
	return GetErrorString(GetLastError());
}

GraphicsException::GraphicsException(HRESULT hr, const char* expr_str)
{
	this->m_error_msg = GetErrorString(hr);
	this->m_expression = expr_str;
}