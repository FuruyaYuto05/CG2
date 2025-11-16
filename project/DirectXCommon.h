#pragma once

// 必要な前方宣言やインクルードを忘れずに
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>          // ComPtrを使うために必要
#include <dxcapi.h>
#include <Windows.h>      // HANDLE用
#include <cstdint>
#include "WinApp.h"

// ComPtrのために名前空間を省略せず記述するために、ここでは using namespace は使わない

class DirectXCommon
{
public:
	void Initialize(WinApp* winApp);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		UINT numDescriptors,
		bool shaderVisible);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
		uint32_t descriptorSize,
		uint32_t index);

	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
		uint32_t descriptorSize,
		uint32_t index);

	/// <summary>
	/// SRVの指定番号のCPUディスクリプタハンドルを取得する
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index) const;

	/// <summary>
	/// SRVの指定番号のGPUディスクリプタハンドルを取得する
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index) const;

	void PreDraw();
	void PostDraw();

private:

	WinApp* winApp_ = nullptr;

	// --- DirectX 基盤オブジェクト (ComPtr へ変更) ---
	
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;  //  ComPtrへ変更
	
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;   //  ComPtrへ変更
	Microsoft::WRL::ComPtr<ID3D12Device> device_;       //  ComPtrへ変更
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_; //  ComPtrへ変更
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_; //  ComPtrへ変更
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_; //  ComPtrへ変更
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_; //  ComPtrへ変更
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2]; //  ComPtrへ変更

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_; //  ComPtrへ変更
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_; //  ComPtrへ変更

	// --- Fence (ComPtr へ変更) ---
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_; //  ComPtrへ変更
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_ = nullptr;

	// --- DXCompiler (ComPtr へ変更) ---
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_; //  ComPtrへ変更
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_; // ComPtrへ変更
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_; //  ComPtrへ変更

	// --- RTV Handles (生ポインタのまま) ---
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	// --- depthStencilResource_ ---
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

	// --- RTV/SRV/DSVのディスクリプタサイズ ---
	uint32_t rtvDescriptorSize_ = 0; // 
	uint32_t srvDescriptorSize_ = 0; // 
	uint32_t dsvDescriptorSize_ = 0; // 

	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect_;


	// --- 新しいプライベート関数 (Initializeから分割) ---
	
	void InitializeDevice();
	void InitializeCommand();     // コマンド関連 (キュー、アロケータ、リスト) の作成
	void InitializeDescriptorHeaps();
	void InitializeSwapchain();   // スワップチェーン、RTVヒープ、RTVの作成
	void InitializeRTV();         // RTVの設定と作成
	void InitializeDepthBufferResource(); 
	void InitializeDSV();               
	void InitializeFence();       // フェンスの作成
	void InitializeViewport();
	void InitializeScissor(); 
	void InitializeDXC();         // DXCコンパイラの初期化
	void InitializeImGui();       // ImGuiのDirectX部分の初期化


};
