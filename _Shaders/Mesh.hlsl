#include "RenderingNode.hlsli"
#include "Shadow.hlsl"

struct VertexInput
{
	float4 Position : POSITION;
	float2 Uv : UV;
	float3 Normal : Normal;
	float3 Tangent : TANGENT;
};


PixelInput VS(VertexInput input)
{
	PixelInput output;
	output.Position = WorldPosition(input.Position);
	output.wPosition = output.Position;
	output.Position = VPPosition(output.Position);

	output.Uv = input.Uv;
	output.Normal = mul(input.Normal, (float3x3)CB_World.World);
	output.Tangent = mul(input.Tangent, (float3x3)CB_World.World);
	output.BiNormal = cross(output.Normal, output.Tangent);
	output.ViewPos = ViewPosition();

	return output;
}


ShadowPixelInput VS_Shadow(ShadowVertexInput input)
{
	ShadowPixelInput output;
	output.Position = WorldPosition(input.Position);
	output.wPosition = output.Position;
	output.Position = VPPosition(output.Position);

	output.Uv = input.Uv;
	output.Normal = mul(input.Normal, (float3x3)CB_World.World);
	output.Tangent = mul(input.Tangent, (float3x3)CB_World.World);
	output.BiNormal = cross(output.Normal, output.Tangent);
	output.ViewPos = ViewPosition();

	output.sPosition = WorldPosition(input.Position);
	output.sPosition = mul(output.sPosition, ShadowView);
	output.sPosition = mul(output.sPosition, ShadowProjection);

	return output;
}


float4 PS(PixelInput input) : SV_TARGET0
{
	float3 diffuse = diffuseMap.Sample(diffuseSamp, input.Uv).rgb;
	float4 result = ComputeLight(input.Normal, input.ViewPos, input.wPosition);
	//float4 emissive = ComputeEmissive(input.Normal, input.wPosition);
	//return float4(1, 1, 0, 1);
	return float4(diffuse,1)*result;
}