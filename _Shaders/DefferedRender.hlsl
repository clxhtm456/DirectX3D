#include "RenderingNode.hlsli"



void Texture(inout float4 color, Texture2D t, float2 uv, SamplerState samp)
{
    float4 sampling = t.Sample(samp, uv);
    
    color = color * sampling;
}

void Texture(inout float4 color, Texture2D t, float2 uv)
{
    Texture(color, t, uv, diffuseSamp);
}

void NormalMapping(inout float4 diffuse ,float2 uv, float3 normal, float3 tangent, SamplerState samp)
{
    float4 map = normalMap.Sample(samp, uv);
    float3 direction = -CB_Light.Direction;
    float4 result = float4(0,0,0,0);

    [flatten]
    if (any(map) == false)
        return;

    float3 N = normalize(normal);
    float3 T = normalize(tangent - dot(tangent, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);


    float3 coord = map.rgb * 2.0f - 1.0f;

    coord = mul(coord, TBN);
    
    diffuse *= saturate(dot(coord, direction));
}

void NormalMapping(inout float4 diffuse, float2 uv, float3 normal, float3 tangent)
{
    NormalMapping(diffuse,uv, normal, tangent, diffuseSamp);

}



struct PixelOutput_PackGBuffer
{
    float4 Diffuse : SV_Target0;
    float4 Specular : SV_Target1;
    float4 Emissive : SV_Target2;
    float4 Normal : SV_Target3;
    float4 Tangent : SV_Target4;
};


PixelOutput_PackGBuffer PS(PixelInput input)
{
    float4 diffuse = CB_Material.Diffuse;
    float4 specular = CB_Material.Specular;

    Texture(diffuse, diffuseMap, input.Uv);
    NormalMapping(diffuse,input.Uv, input.Normal, input.Tangent);
    Texture(specular, specularMap, input.Uv);

    PixelOutput_PackGBuffer output;
    
    output.Diffuse = diffuse;
    output.Specular = specular;
    output.Emissive = CB_Material.Emissive;
    output.Normal = float4(input.Normal, 1); //-1~1사이 값을 다시 0~1사이 값으로 변환
    output.Tangent = float4(input.Tangent, 1);

    return output;
}