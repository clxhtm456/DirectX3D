#include "RenderingNode.hlsli"

struct PixelInput
{
	float4 Position : SV_POSITION;
	float4 wPosition : POSITION1;
	float2 Uv : UV;
	float3 Normal : Normal;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
	float3 ViewPos : VIEWPOS;
};

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

