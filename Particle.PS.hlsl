#include "Particle.hlsli"


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
    
    float32_t4 transformedUV = null(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    output.color = gMaterial.color * textureColor;
 
    if (textureColor.a == 0.0)
    {
        discard; // ピクセルを破棄し、後続の処理（深度書き込み、色合成など）を行わない
    }
    
 
    
    return output;
}