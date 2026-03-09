#include "Camera.h"
#include "WinApp.h" 

// --- 変更：初期化子リストを使った書き方 ---
Camera::Camera()
    : transform({ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -5.0f} }) // ※スライドは0ですが、見えなくなるのを防ぐため -5.0f のままにしています
    , fovY(0.45f)
    , aspectRatio(float(WinApp::kClientWidth) / float(WinApp::kClientHeight))
    , nearClip(0.1f)
    , farClip(100.0f)
    , worldMatrix(Math::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate))
    , viewMatrix(Math::Inverse(worldMatrix))
    , projectionMatrix(Math::MakePerspectiveFovMatrix(fovY, aspectRatio, nearClip, farClip))
    , viewProjectionMatrix(Math::Multiply(viewMatrix, projectionMatrix))
{
    // 中身は空でOKです
}

void Camera::Update()
{
    // ビュー行列の計算
    worldMatrix = Math::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
    viewMatrix = Math::Inverse(worldMatrix);

    // プロジェクション行列の計算
    projectionMatrix = Math::MakePerspectiveFovMatrix(fovY, aspectRatio, nearClip, farClip);

    // ビュー行列とプロジェクション行列の合成
    viewProjectionMatrix = Math::Multiply(viewMatrix, projectionMatrix);
}