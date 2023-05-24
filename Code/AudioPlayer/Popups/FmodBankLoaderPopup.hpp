#pragma once

#include <functional>
#include <string>

class BankLoaderPopup
{
public:
	static void CallOpenFileDialog(const wchar_t* title, std::string& output_str);
	static void Open(const std::function<void(const std::string&, const std::string&)> v_callback_ptr);
	static void Draw();

	BankLoaderPopup() = delete;
	BankLoaderPopup(const BankLoaderPopup&) = delete;
	BankLoaderPopup(BankLoaderPopup&&) = delete;
	~BankLoaderPopup() = delete;

private:
	inline static bool ShouldOpen = false;

	inline static std::string BankPath;
	inline static std::string BankStringsPath;

	inline static std::function<void(const std::string&, const std::string&)> Callback = nullptr;
};