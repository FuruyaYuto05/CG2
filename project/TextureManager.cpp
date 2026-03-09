#include "TextureManager.h"
#include "DirectXCommon.h"
#include "StringUtility.h"
#include "SrvManager.h"
#include <cassert>

TextureManager* TextureManager::instance = nullptr;

// kSRVIndexTop は完全に廃止なので削除しました

TextureManager* TextureManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new TextureManager();
    }
    return instance;
}

void TextureManager::SetDirectXCommon(DirectXCommon* dxCommon)
{
    dxCommon_ = dxCommon;
}

void TextureManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
    // reserve も unordered_map では不要なので削除
    dxCommon_ = dxCommon;
    srvManager_ = srvManager;
}

void TextureManager::Finalize()
{
    delete instance;
    instance = nullptr;
}

// ============================
//   テクスチャ読み込み処理
// ============================
void TextureManager::LoadTexture(const std::string& filePath)
{
    // --- ★変更（スライド通り）：重複読み込みチェック ---
    // contains() 一発で検索できるようになりました！
    if (textureDatas.contains(filePath)) {
        return;
    }

    assert(srvManager_->CanAllocate());

    // --- ファイル読み込み ---
    DirectX::ScratchImage image{};
    std::wstring filePathW = StringUtility::ConvertString(filePath);

    HRESULT hr = DirectX::LoadFromWICFile(
        filePathW.c_str(),
        DirectX::WIC_FLAGS_FORCE_SRGB,
        nullptr,
        image
    );
    assert(SUCCEEDED(hr));

    DirectX::ScratchImage mipImages{};
    hr = DirectX::GenerateMipMaps(
        image.GetImages(),
        image.GetImageCount(),
        image.GetMetadata(),
        DirectX::TEX_FILTER_SRGB,
        0,
        mipImages
    );
    assert(SUCCEEDED(hr));

    // --- テクスチャデータ追加 ---
    TextureData& textureData = textureDatas[filePath];

    // --- データ書き込み ---
    // filePath の代入はキーになっているため不要になりました
    textureData.metadata = mipImages.GetMetadata();
    textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);

    // --- SRV ハンドル計算 ---
    // SrvManager に「次空いてる番号ちょうだい！」とお願いする
    textureData.srvIndex = srvManager_->Allocate();
    textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
    textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

    // --- SRV の生成 ---
    // 以前SrvManagerに作った専用の関数を使ってSRVを生成する！
    srvManager_->CreateSRVforTexture2D(
        textureData.srvIndex,
        textureData.resource.Get(),
        textureData.metadata.format,
        UINT(textureData.metadata.mipLevels)
    );

    // --- GPU へテクスチャデータ転送 ---
    dxCommon_->UploadTextureData(textureData.resource, mipImages);
}

// ============================
//   各種ゲッターの修正
// ============================
// SRVインデックスの取得
uint32_t TextureManager::GetSrvIndex(const std::string& filePath)
{
    // 読み込み済みテクスチャを検索
    if (textureDatas.contains(filePath)) {
        return textureDatas[filePath].srvIndex;
    }

    // 見つからなかったら 0 を返して停止（assert）
    assert(0);
    return 0;
}

// GPUハンドルの取得
D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{
    // filePathからSRVインデックスを取得して、SrvManagerにお願いする
    uint32_t srvIndex = GetSrvIndex(filePath);
    return srvManager_->GetGPUDescriptorHandle(srvIndex);
}

// メタデータの取得
const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
    // filePathをキーにして検索し、見つかったらメタデータを返す
    if (textureDatas.contains(filePath)) {
        return textureDatas[filePath].metadata;
    }

    assert(0);
    return textureDatas.begin()->second.metadata; // エラー回避用のダミーリターン
}