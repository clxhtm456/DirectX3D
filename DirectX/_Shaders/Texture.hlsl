#include "Header.hlsli"

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

Texture2D diffuseMap : register(t0);
SamplerState diffuseSamp : register(s0);

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.Position =input.Position;

	output.Uv = input.Uv;

	return output;
}

float4 PS(VertexOutput input) : SV_TARGET
{
	float4 result = diffuseMap.Sample(diffuseSamp, input.Uv);
    return result;
}