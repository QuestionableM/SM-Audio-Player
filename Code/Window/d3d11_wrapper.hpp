#pragma once

#include "windows_include.hpp"
#include <d3d11.h>

class Renderer
{
public:
	static bool Create(HWND out_wnd);
	static void CreateRenderTarget();
	static void DestroyRenderTarget();
	static void Resize(const unsigned int& width, const unsigned int& height);
	static void Destroy();

	inline static IDXGISwapChain* GetSwapChain() { return Renderer::SwapChain; }
	inline static ID3D11DeviceContext* GetContext() { return Renderer::DeviceContext; }
	inline static ID3D11Device* GetDevice() { return Renderer::Device; }
	inline static ID3D11RenderTargetView* GetMainRenderTargetView() { return Renderer::MainRenderTargetView; }
	inline static D3D_FEATURE_LEVEL GetFeatureLevel() { return Renderer::FeatureLevel; }

	Renderer() = delete;
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	~Renderer() = delete;

private:
	inline static IDXGISwapChain* SwapChain = nullptr;
	inline static ID3D11DeviceContext* DeviceContext = nullptr;
	inline static ID3D11Device* Device = nullptr;
	inline static ID3D11RenderTargetView* MainRenderTargetView = nullptr;
	inline static D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_10_0;
};