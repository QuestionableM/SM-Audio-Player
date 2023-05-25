#pragma once

#include <string>

class InfoPopup
{
public:
	static void Open(const std::string& title, const std::string& message);
	static void Draw();

private:
	inline static bool ShouldOpen = false;

	inline static std::string PopupTitle;
	inline static std::string PopupMessage;
};