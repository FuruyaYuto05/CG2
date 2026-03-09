#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <wrl.h>
#include <d3d12.h>
#include "externals/DirectXTex/DirectXTex.h"

class DirectXCommon; // 前方宣言
class SrvManager;

class TextureManager
{
private:
    static TextureManager* instance;

    TextureManager() = default;
    ~TextureManager() = default;

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    // ★ SRVインデックスの開始番号（ImGuiが0番を使うので1から）
    static uint32_t kSRVIndexTop;


    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_ = nullptr;


    struct TextureData {
        std::string filePath;
        DirectX::TexMetadata metadata;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        uint32_t srvIndex;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU{};
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU{};
    };

    //std::vector<TextureData> textureDatas;
    std::unordered_map<std::string, TextureData> textureDatas;

public:
    static TextureManager* GetInstance();
    void SetDirectXCommon(DirectXCommon* dxCommon);

    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
    void Finalize();

    void LoadTexture(const std::string& filePath);

 

    // メタデータを取得
    const DirectX::TexMetadata& GetMetaData(const std::string& filePath);

    // SRVインデックスの取得（名前も GetTextureIndexByFilePath から短く変更）
    uint32_t GetSrvIndex(const std::string& filePath);

    // GPUハンドルの取得
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);
};
