#include "RenderingNode.hlsli"

struct VertexInput
{
	float4 Position : POSITION;
	float2 Uv : UV;
	float3 Normal : NORMAL;
};

struct VertexOutput
{
	float4 Position : SV_POSITION;
	float2 Uv : UV;
	float3 Normal : NORMAL;
};

struct BrushDesc
{
	float4 Color;
	float3 Location;
	uint Type;
	uint Range;
};

cbuffer Brush : register(b2)
{
	BrushDesc Brush;
}

struct LineDesc
{
	float4 Color;
	uint Visible;
	float Thickness;
	float Size;
};

cbuffer Line : register(b3)
{
	LineDesc Line;
}

Texture2D BaseMap : register(t0);
SamplerState BaseSamp : register(s0);

Texture2D LayerMap : register(t1);
SamplerState LayerSamp : register(s1);

Texture2D AlphaMap : register(t2);
SamplerState AlphaSamp : register(s2);

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.Position = WorldPosition(input.Position);
	output.Position = VPPosition(output.Position);
	//output.Position = input.Position;

	output.Uv = input.Uv;
	output.Normal = mul(input.Normal, (float3x3)CB_World.World);

	return output;
}

float4 PS(VertexOutput input) : SV_TARGET
{
	float3 diffuse = BaseMap.Sample(BaseSamp, input.Uv).rgb;
	//float NdotL = dot(normalize(input.Normal), -GlobalLight.Direction);

	float NdotL = dot(normalize(input.Normal), -CB_Light.Direction);

	return float4(diffuse * NdotL, 1);
}