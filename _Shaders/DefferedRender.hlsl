#include "Header.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseSamp : register(s0);

struct VertexInput
{
	float4 Position : POSITION;
	float2 Uv : UV;
	float3 Normal : Normal;
	float3 Tangent : TANGENT;
};

struct VertexOutput
{
	float4 Position : SV_POSITION;
	float2 Uv : UV;
	float3 Normal : Normal;
	float3 Tangent : TANGENT;
};


VertexOutput VS(VertexInput input)
{
}

float4 PS(VertexOutput input) : SV_TARGET
{
}