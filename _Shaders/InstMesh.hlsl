#include "RenderingNode.hlsli"
#include "Shadow.hlsl"

struct VertexInput
{
	float4 Position : POSITION;
	float2 Uv : UV;
	float3 Normal : Normal;
	float3 Tangent : TANGENT;

	matrix Transform : INSTANCE;
};

struct ShadowVertexInput
{
    float4 Position : POSITION;
    float2 Uv : UV;
    float3 Normal : Normal;
    float3 Tangent : TANGENT;

    matrix Transform : INSTANCE;
};

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


PixelInput VS(VertexInput input)
{
	PixelInput output;
	output.Position = mul(input.Position, input.Transform);
	output.wPosition = output.Position;
	output.Position = VPPosition(output.Position);

	output.Uv = input.Uv;
	output.Normal = mul(input.Normal, (float3x3)input.Transform);
	output.Tangent = mul(input.Tangent, (float3x3)input.Transform);
	output.BiNormal = cross(output.Normal, output.Tangent);
	output.ViewPos = ViewPosition();

	return output;
}


ShadowPixelInput VS_Shadow(ShadowVertexInput input)
{
	ShadowPixelInput output;
	output.Position = mul(input.Position, input.Transform);
	output.wPosition = output.Position;
	output.Position = VPPosition(output.Position);

	output.Uv = input.Uv;
	output.Normal = mul(input.Normal, (float3x3)input.Transform);
	output.Tangent = mul(input.Tangent, (float3x3)input.Transform);
	output.BiNormal = cross(output.Normal, output.Tangent);
	output.ViewPos = ViewPosition();

    output.sPosition = mul(input.Position, input.Transform);
	output.sPosition = mul(output.sPosition, ShadowView);
	output.sPosition = mul(output.sPosition, ShadowProjection);

	return output;
}



float4 PS(PixelInput input) : SV_TARGET0
{
	float4 diffuse = CB_Material.Diffuse;
	float4 specular = CB_Material.Specular;

	Texture(diffuse, diffuseMap, input.Uv);
	NormalMapping(diffuse,input.Uv, input.Normal, input.Tangent);
	Texture(specular, specularMap, input.Uv);

	MaterialDesc result = MakeMaterial();
	MaterialDesc output = MakeMaterial();

	ComputeLight_Material(output, input.Normal, input.wPosition);
	AddMaterial(result, output);

	//return diffuse;
	//return float4(MaterialToColor(result), 1.0f);
	return diffuse*float4(MaterialToColor(result), 1.0f);
}