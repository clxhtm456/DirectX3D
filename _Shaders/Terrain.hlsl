#include "Header.hlsli"

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

Texture2D HeightMap : register(t0);
SamplerState HeightSamp : register(s0);

Texture2D BaseMap : register(t1);
SamplerState BaseSamp : register(s1);

Texture2D LayerMap : register(t2);
SamplerState LayerSamp : register(s2);

Texture2D AlphaMap : register(t3);
SamplerState AlphaSamp : register(s3);

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.Position = WorldPosition(input.Position);
	output.Position = ViewProjection(output.Position);
	//output.Position = input.Position;

	output.Uv = input.Uv;
	output.Normal = input.Normal;

	return output;
}

float4 PS(VertexOutput input) : SV_TARGET
{
	float3 diffuse = BaseMap.Sample(BaseSamp, input.Uv).rgb;

	//float NdotL = dot(normalize(input.Normal), -GlobalLight.Direction);

	return float4(diffuse,1);
	//return float4(1, 0, 0, 1);
}