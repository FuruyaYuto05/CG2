#include "Object3dCommon.h"
#include "DirectXCommon.h" // ← 追加: DirectXCommonの機能を使うために必要

// 引数で dxCommon を受け取るように変更
void Object3dCommon::Initialize(DirectXCommon* dxCommon)
{
    // 引数で受け取ってメンバ変数に記録する
    dxCommon_ = dxCommon;

    // グラフィックスパイプラインの生成()を呼び出す
    CreateGraphicsPipeline();
}

void Object3dCommon::CreateRootSignature()
{
    // TODO: 後でmain.cppから処理をコピーしてくる
}

// ... (これまでの関数の実装) ...

void Object3dCommon::CreateGraphicsPipeline()
{
    // スライドの指示：「グラフィックスパイプラインの生成処理の中で最初にルートシグネチャの作成関数を呼び出す」
    CreateRootSignature();

    // TODO: 後でmain.cppから処理をコピーしてくる
}

// ↓ここから追加
void Object3dCommon::SetCommonDrawSetting()
{
    // コマンドリストの取得
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // ルートシグネチャをセットするコマンド
    commandList->SetGraphicsRootSignature(rootSignature_);
    // グラフィックスパイプラインステートをセットするコマンド
    commandList->SetPipelineState(graphicsPipelineState_);
    // プリミティブトポロジーをセットするコマンド
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
// ↑ここまで追加

