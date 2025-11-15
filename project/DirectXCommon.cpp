#include "DirectXCommon.h"
#include "WinApp.h"
#include "Logger.h"           
#include "StringUtility.h"    

// 必要な標準/Windowsヘッダー
#include <cassert>
#include <string>
#include <format>
#include <sstream>
#include <Windows.h>
#include <dxgidebug.h> 

// DirectX関連の.libを移植
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

// --------------------------------------------------------------------------------
// ヘルパー関数
// --------------------------------------------------------------------------------

// 【修正反映】CreateDescriptorHeapの実装
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(
	D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// device メンバ変数を使用
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));
	assert(SUCCEEDED(hr));

	return descriptorHeap;
}

ID3D12Resource* DirectXCommon::CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height)
{
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}


/// <summary>
/// SRVの指定番号のCPUディスクリプタハンドルを取得する
/// </summary>
D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVCPUDescriptorHandle(uint32_t index)
{
	// メンバ変数を渡して static メンバ関数を呼び出す
	return GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, index);
}

/// <summary>
/// SRVの指定番号のGPUディスクリプタハンドルを取得する
/// </summary>
D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVGPUDescriptorHandle(uint32_t index)
{
	// メンバ変数を渡して static メンバ関数を呼び出す
	return GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, index);
}


// --------------------------------------------------------------------------------
// Private Methods の実装
// --------------------------------------------------------------------------------

void DirectXCommon::CreateDevice()
{
	HRESULT hr;
	hr = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
	assert(SUCCEEDED(hr));
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(useAdapter.GetAddressOf())) !=
		DXGI_ERROR_NOT_FOUND; ++i) {
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Logger::Log(StringUtility::ConvertString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter.Reset();
	}
	assert(useAdapter);
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0 };
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(device.GetAddressOf()));
		if (SUCCEEDED(hr)) {
			Logger::Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	assert(device);
	Logger::Log("Compleate create D3D12Device!!!\n");
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf())))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	}
#endif // _DEBUG
}

void DirectXCommon::CreateCommandObjects()
{
	HRESULT hr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue.GetAddressOf()));
	assert(SUCCEEDED(hr));
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf()));
	assert(SUCCEEDED(hr));
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateDescriptorHeaps()
{
	// 1. 各種DescriptorSizeを取得 
	descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	// 2. RTV用ディスクリプタヒープ生成 (device引数を削除したCreateDescriptorHeapを使用)
	rtvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// 3. SRV用ディスクリプタヒープ生成
	srvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	// 4. DSV用ディスクリプタヒープ生成
	dsvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
}

void DirectXCommon::CreateSwapChain()
{
	HRESULT hr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = WinApp::kClientWidth;
	swapChainDesc.Height = WinApp::kClientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	hr = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.Get(),
		winApp->GetHwnd(),
		&swapChainDesc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(
	const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
	uint32_t descriptorSize,
	uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// オフセット計算: サイズを uint64_t にキャストして計算することで警告 C4267 を回避
	handle.ptr += (static_cast<uint64_t>(descriptorSize) * index);
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(
	const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
	uint32_t descriptorSize,
	uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	// オフセット計算
	handle.ptr += (static_cast<uint64_t>(descriptorSize) * index);
	return handle;
}

void DirectXCommon::CreateRTV()
{
	HRESULT hr;
	const uint32_t kNumBackBuffers = static_cast<uint32_t>(swapChainResources.size()); // バックバッファの数は2

	// 【スライドの内容】スワップチェーンからリソースを引っ張ってくる
	for (uint32_t i = 0; i < kNumBackBuffers; ++i) {
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainResources[i]));
		assert(SUCCEEDED(hr));
	}

	// RTV用の設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// RTVハンドルの開始位置を取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// 【スライドの内容】裏/表の2つ分を for ループで処理
	for (uint32_t i = 0; i < kNumBackBuffers; ++i) {
		// RTVハンドルの取得（i番目のハンドル位置を計算）
		rtvHandles[i].ptr = rtvStartHandle.ptr + (static_cast<uint64_t>(descriptorSizeRTV) * i);

		// レンダーターゲットビューの生成
		device->CreateRenderTargetView(swapChainResources[i].Get(), &rtvDesc, rtvHandles[i]);
	}
}

void DirectXCommon::CreateDepthBuffer()
{
	// 1. 深度バッファリソースの生成
	depthStencilResource = CreateDepthStencilTextureResource(
		device.Get(),
		WinApp::kClientWidth,
		WinApp::kClientHeight
	);

	// 3. DSVの生成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	device->CreateDepthStencilView(
		depthStencilResource,
		&dsvDesc,
		dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
	);
}


void DirectXCommon::CreateFence()
{
	HRESULT hr;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

void DirectXCommon::InitializeViewport()
{
	// ビューポート矩形の設定
	// クライアント領域のサイズと一致させる
	viewport.Width = (float)WinApp::kClientWidth;
	viewport.Height = (float)WinApp::kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

void DirectXCommon::InitializeScissorRect()
{
	// シザーリング矩形の設定
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = WinApp::kClientWidth;
	scissorRect.bottom = WinApp::kClientHeight;
}

void DirectXCommon::CreateDXCCompiler()
{
	HRESULT hr;

	// DXCユーティリティの生成
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils.GetAddressOf()));
	assert(SUCCEEDED(hr));

	// DXCコンパイラの生成
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler.GetAddressOf()));
	assert(SUCCEEDED(hr));

	// デフォルトインクルードハンドラの生成
	hr = dxcUtils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf());
	assert(SUCCEEDED(hr));
}


void DirectXCommon::ImGuiInitialize()
{
	// 1. バージョンチェック, コンテキストの生成
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// 2. スタイルの設定
	ImGui::StyleColorsDark();

	// 3. Win32用の初期化 (ウィンドウハンドルを使用)
	ImGui_ImplWin32_Init(winApp->GetHwnd());

	// 4. DirectX12用の初期化
	// device.Get() : デバイス
	// 2 : バックバッファ数 (swapChainResourcesのサイズ)
	// DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : レンダーターゲットフォーマット
	// rtvDescriptorHeap.Get() : RTVヒープ
	// rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() : CPUハンドル開始位置
	// rtvDescriptorHeap->GetGPUDescriptorHandleForHeapStart() : GPUハンドル開始位置
	ImGui_ImplDX12_Init(
		device.Get(),
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		rtvDescriptorHeap.Get(),
		rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		rtvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);
}

// --------------------------------------------------------------------------------
// Initialize と Finalize のメイン処理
// --------------------------------------------------------------------------------

void DirectXCommon::Initialize(WinApp* winApp)
{
	assert(winApp);
	this->winApp = winApp;

	CreateDevice();
	CreateCommandObjects();
	CreateDescriptorHeaps();
	CreateSwapChain();
	CreateRTV();
	CreateDepthBuffer();
	CreateFence();
	InitializeScissorRect();
	InitializeViewport();
	CreateDXCCompiler();
	ImGuiInitialize();

}

void DirectXCommon::Finalize()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// Rawポインタの解放
	//if (swapChainResources[0]) { swapChainResources[0]->Release(); swapChainResources[0] = nullptr; }
	//if (swapChainResources[1]) { swapChainResources[1]->Release(); swapChainResources[1] = nullptr; }

	if (depthStencilResource) { depthStencilResource->Release(); depthStencilResource = nullptr; }

#ifdef _DEBUG
	// Live Object Report
	Microsoft::WRL::ComPtr<IDXGIDebug> debug;
	if (SUCCEEDED(DXGIGetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf())))) {
		// debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	}
#endif
}