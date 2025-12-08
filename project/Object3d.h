#pragma once
#include <string>
#include <vector>
#include "Math.h"

// 前方宣言
class Object3dCommon;

class Object3d
{
public:
    // 初期化
    void Initialize(Object3dCommon* object3dCommon);

private:
    Object3dCommon* object3dCommon_ = nullptr;

    struct VertexData {
       Math::Vector4 position;
       Math::Vector2 texcoord;
       Math::Vector3 normal;
    };

    struct MaterialData {
        std::string textureFilePath;
    };

    struct ModelData {
        std::vector<VertexData> vertices;
        MaterialData material;
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

};
