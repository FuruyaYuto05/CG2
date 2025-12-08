#include "Object3d.h"
#include "Object3dCommon.h"

// 引数で受け取ってメンバ変数に記録する
void Object3d::Initialize(Object3dCommon* object3dCommon)
{
    this->object3dCommon_ = object3dCommon;

    // モデル読み込み
    modelData_ = LoadObjFile("resources", "plane.obj");

    // ----------------------------------------
    // 頂点データの初期化（スライド内容）
    // ----------------------------------------

    // 頂点数
    uint32_t vertexCount = (uint32_t)modelData_.vertices.size();
    uint32_t bufferSize = sizeof(VertexData) * vertexCount;

    // バッファリソースを作成
    vertexBuffer_ = object3dCommon_->GetDxCommon()->CreateBufferResource(bufferSize);

    // バッファにデータを書き込むためのアドレスを取得
    vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

    // Objから読み込んだデータをコピー
    for (uint32_t i = 0; i < vertexCount; i++) {
        vertexData_[i] = modelData_.vertices[i];
    }

    // バッファビューの作成
    vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = bufferSize;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);
}
