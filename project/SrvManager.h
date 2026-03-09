#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <dxgiformat.h>

class DirectXCommon;

class SrvManager
{
private:
    DirectXCommon* directXCommon = nullptr;

    static const uint32_t kMaxSRVCount;
    uint32_t descriptorSize;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    // --- ★ここから追加（スライド通り） ---
    // 次に使用するSRVインデックス
    uint32_t useIndex = 0;
    // --- 追加ここまで ---

public:
    void Initialize(DirectXCommon* dxCommon);

    void PreDraw();

    void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);


    // --- ★ここから追加（スライド通り） ---
    // SRV確保関数
    uint32_t Allocate();
    // --- 追加ここまで ---

    bool CanAllocate();

    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

    // SRV生成 (テクスチャ用)
    void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);

    // SRV生成 (Structured Buffer用)
    void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
};