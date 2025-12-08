#include "Object3dCommon.h"

void Object3dCommon::Initialize(DirectXCommon* dxCommon)
{
    dxCommon_ = dxCommon;
    CreateGraphicsPipeline();
}

void Object3dCommon::SetCommonDrawingSetting()
{
    // コマンドリストを取得
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // ① ルートシグネチャをセット
    commandList->SetGraphicsRootSignature(rootSignature_.Get());

    // ② パイプラインステートをセット
    commandList->SetPipelineState(pipelineState_.Get());

    // ③ プリミティブトポロジをセット
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

