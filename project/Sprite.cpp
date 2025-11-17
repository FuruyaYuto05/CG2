#include "Sprite.h"
// [NEW] SpriteCommon のポインタの定義と関数呼び出しのためにインクルード
#include "SpriteCommon.h" 
#include <cassert> // (ポインタチェックのために追加)
#include "Math.h"
#include <algorithm>


// 初期化
// [FIX] Initialize の引数に SpriteCommon* を追加
void Sprite::Initialize(SpriteCommon* spriteCommon)
{
    // [NEW] スライドの指示通り、引数で受け取ったポインタをメンバ変数に記録する
    this->spriteCommon_ = spriteCommon;

    // ポインタが有効か確認
    assert(this->spriteCommon_ != nullptr);

    CreateVertexData();

    // TODO: 今後、ここにリソース生成のロジックを実装していく
}

// =============================================================
// [NEW] 頂点データとバッファの生成
// =============================================================
void Sprite::CreateVertexData() {
    ID3D12Device* device = spriteCommon_->GetDxCommon()->GetDevice();
    HRESULT hr = S_FALSE;

    // --- 1. VertexResource / IndexResource を作る ---
    const size_t kVertexCount = 4; // 矩形は通常4頂点
    const size_t kIndexCount = 6;  // 矩形は通常6インデックス

    // VertexResource (4頂点)
    vertexResource_ = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * kVertexCount);
    // IndexResource (6インデックス)
    indexResource_ = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * kIndexCount);

    // --- 2. VertexResource / IndexResource にデータを書き込むためのアドレスを取得 ---
    // VertexData* vertexData_ = nullptr;
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    // uint32_t* indexData_ = nullptr;
    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

    // --- 3. VertexData の初期値設定 (main.cpp から移植) ---
    // ※ ここではスプライトのサイズを仮で 128x128 と設定するロジックを移植します
    // main.cppの座標 {0.0f,360.0f}, {0.0f,0.0f}, {640.0f,360.0f}, {640.0f,0.0f}
    float left = 0.0f;
    float top = 0.0f;
    float right = 128.0f; // 仮の幅
    float bottom = 128.0f; // 仮の高さ

    vertexData_[0].position = { left, bottom, 0.0f, 1.0f };    // 左下
    vertexData_[0].texcoord = { 0.0f, 1.0f };
    vertexData_[1].position = { left, top, 0.0f, 1.0f };       // 左上
    vertexData_[1].texcoord = { 0.0f, 0.0f };
    vertexData_[2].position = { right, bottom, 0.0f, 1.0f };   // 右下
    vertexData_[2].texcoord = { 1.0f, 1.0f };
    vertexData_[3].position = { right, top, 0.0f, 1.0f };      // 右上
    vertexData_[3].texcoord = { 1.0f, 0.0f };
    // Normalは一旦0で埋める
    vertexData_[0].normal = { 0.0f, 0.0f, -1.0f };
    vertexData_[1].normal = { 0.0f, 0.0f, -1.0f };
    vertexData_[2].normal = { 0.0f, 0.0f, -1.0f };
    vertexData_[3].normal = { 0.0f, 0.0f, -1.0f };

    // IndexData の設定 (main.cpp から移植)
    indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2; // 1枚目の三角形 (左下、左上、右下)
    indexData_[3] = 1; indexData_[4] = 3; indexData_[5] = 2; // 2枚目の三角形 (左上、右上、右下)

    // --- 4. VertexBufferView / IndexBufferView を作成する ---
    // VertexBufferView
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * kVertexCount;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    // IndexBufferView
    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * kIndexCount;
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}