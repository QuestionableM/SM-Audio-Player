#include "Window.hpp"

#include <dwmapi.h>

Window* Window::Create(
	const wchar_t* title,
	const unsigned int& width,
	const unsigned int& height,
	WNDPROC wnd_proc)
{
	WNDCLASSEX wnd_class{
		.cbSize = sizeof(wnd_class),
		.style = CS_CLASSDC,
		.lpfnWndProc = wnd_proc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = GetModuleHandle(nullptr),
		.hIcon = nullptr,
		.hCursor = nullptr,
		.hbrBackground = nullptr,
		.lpszMenuName = nullptr,
		.lpszClassName = L"ReClassImGuiClass",
		.hIconSm = nullptr
	};

	::RegisterClassEx(&wnd_class);

	HWND wnd_handle = ::CreateWindowW(
		wnd_class.lpszClassName,
		title,
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		static_cast<int>(width),
		static_cast<int>(height),
		nullptr,
		nullptr,
		wnd_class.hInstance,
		nullptr
	);

	if (!wnd_handle)
	{
		::UnregisterClass(wnd_class.lpszClassName, wnd_class.hInstance);
		return nullptr;
	}

	Window* v_new_window = new Window();
	v_new_window->m_windowClass = wnd_class;
	v_new_window->m_windowHandle = wnd_handle;
	v_new_window->m_isOpen = true;
	v_new_window->m_width = width;
	v_new_window->m_height = height;
	
	return v_new_window;
}

void Window::SetUserdata(void* userdata)
{
	::SetWindowLongPtr(m_windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(userdata));
}

void Window::ProcessMessages()
{
	MSG v_wndMsg;
	while (::PeekMessage(&v_wndMsg, nullptr, 0, 0, PM_REMOVE))
	{
		if (v_wndMsg.message == WM_QUIT)
		{
			m_isOpen = false;
			continue;
		}

		::TranslateMessage(&v_wndMsg);
		::DispatchMessage(&v_wndMsg);
	}
}

void Window::Open()
{
	::ShowWindow(m_windowHandle, SW_SHOWDEFAULT);
	::UpdateWindow(m_windowHandle);
}

Window::~Window()
{
	if (m_windowHandle)
	{
		::DestroyWindow(m_windowHandle);
	}

	::UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
}