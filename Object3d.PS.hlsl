#include "object3d.hlsli"


struct Material
{
    float32_t4 color;
};
ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransformed);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    output.color = gMaterial.color = textureColor;
    
    // スライドの内容を反映: textureColor.aの値が0.5以下のときにPixelを破棄
    if (output.color.a <= 0.5)
    {
        discard; // ピクセルを破棄し、後続の処理（深度書き込み、色合成など）を行わない
    }
    
    // 破棄されなかったピクセルについて通常通り色を計算
   // output.color = gMaterial.color * textureColor;
    
    return output;
}