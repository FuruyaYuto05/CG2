#pragma once
#include <string>
#include <vector>
#include "Math.h"
#include <wrl.h>
#include <d3d12.h>


// 前方宣言
class Object3dCommon;

class Object3d
{
public:
    // 初期化
    void Initialize(Object3dCommon* object3dCommon);
    void Update();
    void Draw();

private:
    Object3dCommon* object3dCommon_ = nullptr;

    struct VertexData {
       Math::Vector4 position;
       Math::Vector2 texcoord;
       Math::Vector3 normal;
    };

    struct MaterialData {
        std::string textureFilePath;
        uint32_t textureIndex = 0;
    };

    struct ModelData {
        std::vector<VertexData> vertices;
        MaterialData material;
    };

    struct Material {
        Math::Vector4 color;
        int32_t enableLighting;
        float padding[3];
        Math::Matrix4x4 uvTransform;
    };

    struct TransformationMatrix {
        Math::Matrix4x4 WVP;
        Math::Matrix4x4 World;
    };

    // 平行光源データ
    struct DirectionalLight {
        Math::Vector4 color;     // ライトの色
        Math::Vector3 direction; // ライトの向き
        float intensity;         // 光の強さ（または padding）
    };


    // Objファイルのデータ
    ModelData modelData_;

    // mtlファイル読み取り
    static MaterialData LoadMaterialTemplateFile(const std::string directoryPath, const std::string filename);

    // objファイル読み取り
    static ModelData LoadObjFile(const std::string directoryPath, const std::string filename);

    // バッファリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;

    // バッファリソース内のデータを指すポインタ
    VertexData* vertexData_ = nullptr;

    // バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

    // バッファリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

    // バッファリソースのデータを指すポインタ
    Material* materialData_ = nullptr;

    // 座標変換行列用定数バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;

    // 座標変換行列用定数バッファの中身を指すポインタ
    TransformationMatrix* transformationMatrixData_ = nullptr;

    // Transform（位置・回転・スケール）
    Math::Transform transform_;
    Math::Transform cameraTransform_;


};
