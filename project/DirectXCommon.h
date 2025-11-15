#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>        // Microsoft::WRL::ComPtr を使用
#include <cstdint>
#include <dxcapi.h>
#include "WinApp.h"
#include <array>



class DirectXCommon
{
public:
	DirectXCommon() = default;
	~DirectXCommon() { /* Finalize(); */ }

	void Initialize(WinApp* winApp);
	void Finalize();

	/// <summary>
	/// SRVの指定番号のCPUディスクリプタハンドルを取得する
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// SRVの指定番号のGPUディスクリプタハンドルを取得する
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);


private:

	WinApp* winApp = nullptr;

	// -----------------------------------------------------
	// Private Static Methods (ディスクリプタハンドル取得関数)
	// -----------------------------------------------------

	/// <summary>
	/// 指定番号のCPUディスクリプタハンドルを取得する
	/// </summary>
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
		uint32_t descriptorSize,
		uint32_t index);

	/// <summary>
	/// 指定番号のGPUディスクリプタハンドルを取得する
	/// </summary>
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
		uint32_t descriptorSize,
		uint32_t index);

	// -----------------------------------------------------
	// メンバ変数 (Microsoft::WRL::ComPtr を使用)
	// -----------------------------------------------------

	// DXGI関連
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;

	// Device関連
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;

	// Command関連
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	// 【追加】DXCコンパイラ関連
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;


	// SwapChainとRTV関連
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	//ID3D12Resource* swapChainResources[2] = { nullptr };
	
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;
	
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	// 深度ステンシル関連
	ID3D12Resource* depthStencilResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	// Fence
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	uint64_t fenceValue = 0;

	// ディスクリプタインクリメントサイズ
	uint32_t descriptorSizeRTV = 0;
	uint32_t descriptorSizeSRV = 0;
	uint32_t descriptorSizeDSV = 0;


	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	// -----------------------------------------------------
	// Private Methods (初期化ステップを分割)
	// -----------------------------------------------------

	void CreateDevice();
	void CreateCommandObjects();
	void CreateDescriptorHeaps();
	void CreateSwapChain();
	void CreateRTV();
	void CreateDepthBuffer();
	void InitializeViewport();
	void InitializeScissorRect();
	void CreateDXCCompiler();
	void CreateFence();
	void ImGuiInitialize();

	// Helper function
	// 【最終形】ComPtrを返し、deviceはメンバ変数を使用
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	// 深度バッファリソース生成ヘルパー
	ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);
};