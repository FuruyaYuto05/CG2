#include "object3d.hlsli"


struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    
    float32_t shininess;
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

struct Camera
{
    float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

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
        
        // 2. 2乗して拡散反射光の係数（diffuse）とする
        float32_t diffuse = pow(halfLambertRamp, 2.0f);
        
        
        float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        
        
        // 1. 入射光の反射ベクトルを求める (reflect関数)
        // 入射光(direction)と法線(normal)を使って反射方向を計算
        float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));

        // 2. 鏡面反射の強さを求める (R・E の shininess乗)
        // 反射光(reflectLight)と視線(toEye)の内積をとる
        float32_t RdotE = dot(reflectLight, toEye);
        // 負の値にならないようにsaturateし、shininess乗してハイライトの鋭さを調整
        float32_t specularPow = pow(saturate(RdotE), gMaterial.shininess);
        
        // 最終的な出力カラーを計算
       // 拡散反射光（Diffuse）の計算
        float32_t3 diffuseColor = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * diffuse * gDirectionalLight.intensity;

        // 鏡面反射光（Specular）の計算
        // 光源の色 * 光源の強度 * 鏡面反射の強さ
        // ※鏡面反射はテクスチャの色を乗算しないのが一般的です（光そのものの色が反射するため）
        float32_t3 specularColor = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow;

        // 最終的な色 = 拡散反射 + 鏡面反射
        output.color.rgb = diffuseColor + specularColor;
        // 💡 修正箇所2: アルファ成分 (テクスチャとマテリアルのアルファ値のみを適用)
        // ライティング計算（intensityなど）の影響を受けないように分離
        output.color.a = gMaterial.color.a * textureColor.a;
        
    }
    else
    {
        // ライティング無効の場合 (RGBとAを分離する必要なし、単に積算)
        output.color = gMaterial.color * textureColor;
    }
    
    return output;
}