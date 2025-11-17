#pragma once

#include "Math.h" 
#include <d3d12.h>
#include <wrl.h>

// スプライト共通部（SpriteCommon）のポインタ型を使用するため、前方宣言を行う
class SpriteCommon;

//スプライト
class Sprite
{
public:
    // 初期化
    // [FIX] SpriteCommon* を引数として受け取る
    void Initialize(SpriteCommon* spriteCommon);

private:
    //SpriteCommon のポインタをメンバ変数として保持
    SpriteCommon* spriteCommon_ = nullptr; // nullptrlで初期化

    // 頂点データ
    struct VertexData {
        Math::Vector4 position;
        Math::Vector2 texcoord;
        Math::Vector3 normal; // スライドの指示通り、一旦normalも含めておく
    };

    // [NEW] 頂点データ (メンバ変数)

    // バッファリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;   // VertexBuffer
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;    // IndexBuffer

    // バッファリソース内のデータを指すポインタ (マップ先)
    VertexData* vertexData_ = nullptr;
    uint32_t* indexData_ = nullptr;

    // バッファリソースの使い道を補足するバッファビュー
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_;

    void CreateVertexData();
};