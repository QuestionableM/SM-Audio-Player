#include "Application.hpp"

#include "Utils/Exceptions.hpp"
#include "Utils/Console.hpp"

#include "d3d11_wrapper.hpp"
#include "imgui_include.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT ImGuiApplication::ApplicationInitProcedure(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		::SetWindowLongPtr(wnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(ImGuiApplication::ApplicationMainProcedure));
		break;
	default:
		return ::DefWindowProc(wnd, msg, wParam, lParam);
	}

	return 0;
}

LRESULT ImGuiApplication::ApplicationMainProcedure(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(wnd, msg, wParam, lParam))
		return 0;

	ImGuiApplication* v_cur_app = reinterpret_cast<ImGuiApplication*>(::GetWindowLongPtr(wnd, GWLP_USERDATA));

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		{
			if (wParam != SIZE_MINIMIZED)
			{
				const unsigned int v_new_width = (unsigned int)LOWORD(lParam);
				const unsigned int v_new_height = (unsigned int)HIWORD(lParam);

				v_cur_app->Resize(v_new_width, v_new_height);
				v_cur_app->RenderFrame();
			}

			break;
		}
	default:
		return ::DefWindowProc(wnd, msg, wParam, lParam);
	}

	return 0;
}

void ImGuiApplication::InitImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().IniFilename = nullptr;

	ImGui_ImplWin32_Init(m_window->GetHandle());
	ImGui_ImplDX11_Init(Renderer::GetDevice(), Renderer::GetContext());
}

void ImGuiApplication::DestroyImGui()
{
	if (!ImGui::GetCurrentContext())
		return;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool ImGuiApplication::Initialize(const wchar_t* app_name)
{
	m_window = Window::Create(app_name, 1280, 720, ImGuiApplication::ApplicationInitProcedure);
	if (!m_window) return false;

	if (!Renderer::Create(m_window->GetHandle()))
		return false;

	Renderer::Resize(m_window->GetWidth(), m_window->GetHeight());
	ImGuiApplication::InitImGui();

	m_window->SetUserdata(this);
	m_window->Open();

	return true;
}

void ImGuiApplication::Start()
{
	this->OnCreate();

	while (m_window->IsOpen())
	{
		m_window->ProcessMessages();
		this->RenderFrame();
	}
}

bool ImGuiApplication::EnterLoop(const wchar_t* app_name)
{
	try
	{
		if (!this->Initialize(app_name))
			return false;

		this->Start();

		return true;
	}
	catch (const GraphicsException& v_gfx_ex)
	{
		MessageBoxA(m_window->GetHandle(), v_gfx_ex.GetString().c_str(), "Graphics Exception", MB_OK);
	}
	catch (...)
	{
		MessageBoxA(m_window->GetHandle(), "Unknown Error", "Exception", MB_OK);
	}

	return false;
}

void ImGuiApplication::Resize(const unsigned int& width, const unsigned int& height)
{
	m_window->m_width = width;
	m_window->m_height = height;

	Renderer::Resize(width, height);
}

void ImGuiApplication::RenderFrame()
{
	this->BeforeRender();

	//Start the imgui new frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrameCustom();

	//Render user content
	this->Render();

	//Render the filled buffers
	ImGui::Render();

	ID3D11RenderTargetView* v_rtv = Renderer::GetMainRenderTargetView();
	Renderer::GetContext()->OMSetRenderTargets(1, &v_rtv, nullptr);

	float v_reset_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Renderer::GetContext()->ClearRenderTargetView(Renderer::GetMainRenderTargetView(), v_reset_color);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//Present the new frame
	HRESULT hr;
	GFX_TEST_THROW(Renderer::GetSwapChain()->Present(1, 0));

	this->AfterRender();
}

ImGuiApplication::~ImGuiApplication()
{
	if (m_window) delete m_window;

	ImGuiApplication::DestroyImGui();
	Renderer::Destroy();
}