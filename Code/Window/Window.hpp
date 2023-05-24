#pragma once

#include "windows_include.hpp"

class Window
{
	friend class ImGuiApplication;

public:
	static Window* Create(
		const wchar_t* title,
		const unsigned int& width,
		const unsigned int& height,
		WNDPROC wnd_proc);

	void SetUserdata(void* userdata);

	void ProcessMessages();
	void Open();

	inline unsigned int GetWidth() const { return m_width; }
	inline unsigned int GetHeight() const { return m_height; }

	inline HWND GetHandle() const { return m_windowHandle; }
	inline bool IsOpen() const { return m_isOpen; }

	Window() = default;
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	~Window();

private:
	WNDCLASSEX m_windowClass;
	HWND m_windowHandle;

	unsigned int m_width, m_height;
	bool m_isOpen;
};