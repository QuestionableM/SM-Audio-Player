#pragma once

#include "Window.hpp"

class ImGuiApplication
{
public:
	static LRESULT ApplicationInitProcedure(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT ApplicationMainProcedure(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void InitImGui();
	void DestroyImGui();

	bool Initialize(const wchar_t* app_name);
	void Start();

	//Loop with exception handling
	bool EnterLoop(const wchar_t* app_name);

	void Resize(const unsigned int& width, const unsigned int& height);

	virtual void OnCreate() {}

	virtual void BeforeRender() {}
	virtual void AfterRender() {}

	virtual void Render() = 0;
	void RenderFrame();

	ImGuiApplication() = default;
	ImGuiApplication(const ImGuiApplication&) = delete;
	ImGuiApplication(ImGuiApplication&&) = delete;
	virtual ~ImGuiApplication();

protected:
	Window* m_window = nullptr;
	WNDPROC m_procPointer;
};