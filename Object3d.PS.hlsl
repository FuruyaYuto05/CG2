#include "object3d.hlsli"


struct Material
{
    float32_t4 color;
    int32_t enableLighting;
};
ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    if (gMaterial.enableLighting != 0)
    {
        // 正規化された法線ベクトル
        float32_t3 normal = normalize(input.normal);
        // 正規化された光源方向ベクトル (光源から物体への逆方向)
        float32_t3 lightDir = -gDirectionalLight.direction;
        
        // 内積 NdotL は [-1, 1] の範囲を取る
        float32_t NdotL = dot(normal, lightDir);
        
        // --- Half Lambert の実装 ---
        // スライドの式: (0.5 * (n ・ l) + 0.5)^2 を実装
        
        // 1. [-1, 1] を [0, 1] に変換
        float32_t halfLambertRamp = NdotL * 0.5f + 0.5f;
        
        // 2. 2乗して拡散反射光の係数（cos）とする
        float32_t diffuse = pow(halfLambertRamp, 2.0f);
        
        // 最終的な出力カラーを計算
        // (マテリアルカラー * テクスチャカラー) * ライトカラー * 拡散反射光係数 * ライト強度
        output.color = gMaterial.color * textureColor * gDirectionalLight.color * diffuse * gDirectionalLight.intensity;
    }
    else
    {
        // ライティング無効の場合
        output.color = gMaterial.color * textureColor;
    }
    
    return output;
}