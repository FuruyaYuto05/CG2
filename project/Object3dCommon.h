#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "DirectXCommon.h"

class Object3dCommon
{
public:
    // 初期化
    void Initialize(DirectXCommon* dxCommon);

    DirectXCommon* GetDxCommon() const { return dxCommon_; }

    // 共通描画設定
    void SetCommonDrawingSetting();

private:

    DirectXCommon* dxCommon_;

    // ルートシグネチャの作成
    void CreateRootSignature();

    // グラフィックスパイプラインの生成
    void CreateGraphicsPipeline();

    // ルートシグネチャ
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

    // グラフィックスパイプラインステート
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
};
