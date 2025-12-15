#include "Object3d.h"
#include "Object3dCommon.h"

// 引数で受け取ってメンバ変数に記録する
void Object3d::Initialize(Object3dCommon* object3dCommon)
{
    this->object3dCommon_ = object3dCommon;

    // モデル読み込み
    modelData_ = LoadObjFile("resources", "plane.obj");

    // ----------------------------------------
    // 頂点データの初期化
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

    // ----------------------------------------
    // マテリアルの初期化
    // ----------------------------------------

    // マテリアル用定数バッファを作成
    materialResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));

    // バッファにデータを書き込むためのアドレスを取得
    materialResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData_));

    // マテリアルの初期値
    materialData_->color = Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialData_->enableLighting = false;
    materialData_->uvTransform = Math::MakeIdentity4x4();

    // ----------------------------------------
    // 座標変換行列の初期化
    // ----------------------------------------

// 座標変換行列用定数バッファを作成
    transformationMatrixResource_ =
        object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

    // バッファにデータを書き込むためのアドレスを取得
    transformationMatrixResource_->Map(
        0,
        nullptr,
        reinterpret_cast<void**>(&transformationMatrixData_)
    );

    // 単位行列を書き込む
    transformationMatrixData_->WVP = Math::MakeIdentity4x4();
    transformationMatrixData_->World = Math::MakeIdentity4x4();


}
