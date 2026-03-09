#pragma once
#include "Math.h"

class Camera
{
private:
    Math::Transform transform;
    Math::Matrix4x4 worldMatrix;
    Math::Matrix4x4 viewMatrix;

    Math::Matrix4x4 projectionMatrix;
    float fovY;
    float aspectRatio;
    float nearClip;
    float farClip;

    // --- ここから追加 ---
    Math::Matrix4x4 viewProjectionMatrix; // ビュープロジェクション合成行列
    // --- 追加ここまで ---

public:
    Camera();
    void Update();

    // ゲッター群
    const Math::Matrix4x4& GetViewMatrix() const { return viewMatrix; }
    const Math::Matrix4x4& GetProjectionMatrix() const { return projectionMatrix; }
    const Math::Matrix4x4& GetWorldMatrix() const { return worldMatrix; }

    // --- ここから追加 ---
    // Object3d側でWVPを計算する時にこの合成行列を受け取れるようにします
    const Math::Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix; }
    // --- 追加ここまで ---

    const Math::Vector3& GetTranslate() const { return transform.translate; }
    const Math::Vector3& GetRotate() const { return transform.rotate; }

    // セッター群
    void SetTranslate(const Math::Vector3& translate) { transform.translate = translate; }
    void SetRotate(const Math::Vector3& rotate) { transform.rotate = rotate; }

    void SetFovY(float fovY) { this->fovY = fovY; }
    void SetAspectRatio(float aspectRatio) { this->aspectRatio = aspectRatio; }
    void SetNearClip(float nearClip) { this->nearClip = nearClip; }
    void SetFarClip(float farClip) { this->farClip = farClip; }
};