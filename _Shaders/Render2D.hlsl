#include "RenderingNode.hlsli"

cbuffer CB_Render2D : register(b2)
{
	matrix View2D;
	matrix Projection2D;
};

struct VertexInput
{
	float4 Position : POSITION;
	float2 Uv : UV;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV;
};

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.Position = WorldPosition(input.Position);
	output.Position = mul(output.Position, View2D);
	output.Position = mul(output.Position, Projection2D);

	output.Uv = input.Uv;

	return output;
}

float4 PS(VertexOutput input) : SV_TARGET
{
	return diffuseMap.Sample(diffuseSamp, input.Uv);
}