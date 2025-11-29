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

    void Update();

    // [確認] 描画処理メソッド
    void Draw(ID3D12GraphicsCommandList* commandList);

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


    // [NEW] マテリアルデータ
    struct Material {
        Math::Vector4 color;             // 色情報 (Vector4)
        int32_t enableLighting;          // ライティング有効/無効フラグ (int32_t)
        float padding[3];                // パディング (float[3])
        Math::Matrix4x4 uvTransform;     // UV変換行列 (Matrix4x4)
    };

    // [NEW] マテリアル (メンバ変数)

    // バッファリソース (ConstantBuffer)
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

    // バッファリソース内のデータを指すポインタ
    Material* materialData_ = nullptr;


    // [NEW] マテリアルデータとバッファの生成を担うプライベートメソッド
    void CreateMaterial();



    // [NEW] 座標変換行列データ
    struct TransformationMatrix {
        Math::Matrix4x4 WVP;    // World View Projection Matrix
        Math::Matrix4x4 World;  // World Matrix
    };

    // [確認] 座標変換行列 (メンバ変数)

    // バッファリソース (ConstantBuffer)
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;

    // バッファリソース内のデータを指すポインタ
    TransformationMatrix* transformationMatrixData_ = nullptr;

    // トランスフォーム情報
    Math::Transform transform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

    // [NEW] 座標変換行列とバッファの生成を担うプライベートメソッド
    void CreateTransformationMatrix();

    // [NEW] 行列更新処理をカプセル化するためのプライベートメソッド
    void UpdateTransformationMatrix();
};