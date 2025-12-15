#include "Object3d.h"
#include "Object3dCommon.h"
#include "TextureManager.h"
#include <fstream>
#include <sstream>
#include <cassert>


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

    // 平行光源用定数バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;

    // 平行光源用定数バッファの中身を指すポインタ
    DirectionalLight* directionalLightData_ = nullptr;

    // ----------------------------------------
// 平行光源の初期化
// ----------------------------------------

// 平行光源用定数バッファを作成
    directionalLightResource_ =
        object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));

    // バッファにデータを書き込むためのアドレスを取得
    directionalLightResource_->Map(
        0,
        nullptr,
        reinterpret_cast<void**>(&directionalLightData_)
    );

    // 平行光源のデフォルト値
    directionalLightData_->color = Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    directionalLightData_->direction = Math::Vector3(0.0f, -1.0f, 0.0f);
    directionalLightData_->intensity = 1.0f;

    // ----------------------------------------
    // テクスチャ読み込み
    // ----------------------------------------

    TextureManager::GetInstance()->LoadTexture(
        modelData_.material.textureFilePath
    );

    modelData_.material.textureIndex =
        TextureManager::GetInstance()->GetTextureIndexByFilePath(
            modelData_.material.textureFilePath
        );

    // ----------------------------------------
    // Transform の初期化
    // ----------------------------------------

    transform_ = {
        {1.0f, 1.0f, 1.0f},   // scale
        {0.0f, 0.0f, 0.0f},   // rotate
        {0.0f, 0.0f, 0.0f}    // translate
    };

    cameraTransform_ = {
        {1.0f, 1.0f, 1.0f},   // scale
        {-0.3f, 0.0f, 0.0f},  // rotate
        {0.0f, 4.0f, -10.0f}  // translate
    };


}


void Object3d::Update() {

    // World行列
    Math::Matrix4x4 worldMatrix =
        Math::MakeAffineMatrix(
            transform_.scale,
            transform_.rotate,
            transform_.translate
        );

    // カメラ行列
    Math::Matrix4x4 cameraMatrix =
        Math::MakeAffineMatrix(
            cameraTransform_.scale,
            cameraTransform_.rotate,
            cameraTransform_.translate
        );

    // View行列（カメラ行列の逆行列）
    Math::Matrix4x4 viewMatrix = Math::Inverse(cameraMatrix);

    // Projection行列
    Math::Matrix4x4 projectionMatrix =
        Math::MakePerspectiveFovMatrix(
            0.45f,
            float(WinApp::kClientWidth) / float(WinApp::kClientHeight),
            0.1f,
            100.0f
        );

    // GPUに渡す行列を書き込み
    transformationMatrixData_->WVP =
        Math::Multiply(
            Math::Multiply(worldMatrix, viewMatrix),
            projectionMatrix
        );


    transformationMatrixData_->World = worldMatrix;
}


void Object3d::Draw()
{
    // コマンドリスト取得
    ID3D12GraphicsCommandList* commandList =
        object3dCommon_->GetDxCommon()->GetCommandList();

    // -----------------------------
    // VertexBufferView を設定
    // -----------------------------
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // -----------------------------
    // マテリアル CBV を設定（PixelShader）
    // RootParameter[0]
    // -----------------------------
    commandList->SetGraphicsRootConstantBufferView(
        0,
        materialResource_->GetGPUVirtualAddress()
    );

    // -----------------------------
    // 座標変換行列 CBV を設定（VertexShader）
    // RootParameter[1]
    // -----------------------------
    commandList->SetGraphicsRootConstantBufferView(
        1,
        transformationMatrixResource_->GetGPUVirtualAddress()
    );

    // -----------------------------
    // SRV（テクスチャ）を設定
    // RootParameter[2]
    // -----------------------------
    commandList->SetGraphicsRootDescriptorTable(
        2,
        TextureManager::GetInstance()->GetSrvHandleGPU(
            modelData_.material.textureIndex
        )
    );

    // -----------------------------
    // 描画（DrawCall）
    // -----------------------------
    commandList->DrawInstanced(
        static_cast<UINT>(modelData_.vertices.size()),
        1,
        0,
        0
    );
}


// mtlファイル読み取り
Object3d::MaterialData Object3d::LoadMaterialTemplateFile(
    const std::string directoryPath,
    const std::string filename
) {
    MaterialData materialData;
    std::string line;

    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());

    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        if (identifier == "map_Kd") {
            std::string textureFilename;
            s >> textureFilename;
            materialData.textureFilePath =
                directoryPath + "/" + textureFilename;
        }
    }

    return materialData;
}

// objファイル読み取り
Object3d::ModelData Object3d::LoadObjFile(
    const std::string directoryPath,
    const std::string filename
) {
    ModelData modelData;
    std::vector<Math::Vector4> positions;
    std::vector<Math::Vector3> normals;
    std::vector<Math::Vector2> texcoords;
    std::string line;

    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());

    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        if (identifier == "v") {
            Math::Vector4 pos{};
            s >> pos.x >> pos.y >> pos.z;
            pos.w = 1.0f;
            positions.push_back(pos);
        } else if (identifier == "vt") {
            Math::Vector2 uv{};
            s >> uv.x >> uv.y;
            uv.y = 1.0f - uv.y;
            texcoords.push_back(uv);
        } else if (identifier == "vn") {
            Math::Vector3 normal{};
            s >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (identifier == "f") {
            VertexData triangle[3];

            for (int i = 0; i < 3; i++) {
                std::string vertexDef;
                s >> vertexDef;
                std::istringstream v(vertexDef);

                int indices[3];
                for (int j = 0; j < 3; j++) {
                    std::string index;
                    std::getline(v, index, '/');
                    indices[j] = std::stoi(index);
                }

                triangle[i].position = positions[indices[0] - 1];
                triangle[i].texcoord = texcoords[indices[1] - 1];
                triangle[i].normal = normals[indices[2] - 1];
            }

            modelData.vertices.push_back(triangle[2]);
            modelData.vertices.push_back(triangle[1]);
            modelData.vertices.push_back(triangle[0]);
        } else if (identifier == "mtllib") {
            std::string mtlFile;
            s >> mtlFile;
            modelData.material =
                LoadMaterialTemplateFile(directoryPath, mtlFile);
        }
    }

    return modelData;
}
