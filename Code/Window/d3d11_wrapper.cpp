#include "d3d11_wrapper.hpp"

#include "Utils/Exceptions.hpp"

#pragma comment(lib, "d3d11.lib")

bool Renderer::Create(HWND out_wnd)
{
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferDesc.Width = 0;
	swap_chain_desc.BufferDesc.Height = 0;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 144;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = out_wnd;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT v_createDeviceFlags = 0;
#if defined(_DEBUG)
	v_createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL v_featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };
	GFX_TEST_THROW(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		v_createDeviceFlags,
		v_featureLevels,
		sizeof(v_featureLevels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION,
		&swap_chain_desc,
		&Renderer::SwapChain,
		&Renderer::Device,
		&Renderer::FeatureLevel,
		&Renderer::DeviceContext
	));

	Renderer::CreateRenderTarget();
	return true;
}

void Renderer::CreateRenderTarget()
{
	HRESULT hr;
	ID3D11Texture2D* v_backBuffer;

	GFX_TEST_THROW(Renderer::SwapChain->GetBuffer(0, IID_PPV_ARGS(&v_backBuffer)));
	GFX_TEST_THROW(Renderer::Device->CreateRenderTargetView(v_backBuffer, nullptr, &Renderer::MainRenderTargetView));
	
	v_backBuffer->Release();
	v_backBuffer = nullptr;
}

void Renderer::DestroyRenderTarget()
{
	if (Renderer::MainRenderTargetView)
	{
		Renderer::MainRenderTargetView->Release();
		Renderer::MainRenderTargetView = nullptr;
	}
}

void Renderer::Resize(const unsigned int& width, const unsigned int& height)
{
	HRESULT hr;

	Renderer::DestroyRenderTarget();
	GFX_TEST_THROW(Renderer::SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0));
	Renderer::CreateRenderTarget();
}

void Renderer::Destroy()
{
	if (Renderer::Device)
	{
		Renderer::Device->Release();
		Renderer::Device = nullptr;
	}

	if (Renderer::DeviceContext)
	{
		Renderer::DeviceContext->Release();
		Renderer::DeviceContext = nullptr;
	}

	Renderer::DestroyRenderTarget();

	if (Renderer::SwapChain)
	{
		Renderer::SwapChain->Release();
		Renderer::SwapChain = nullptr;
	}
}