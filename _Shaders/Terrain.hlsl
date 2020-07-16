#include "RenderingNode.hlsli"

SamplerState terrainSample : register(s10);

SamplerState Sampler_Filter_Point
{
	Filter = MIN_MAG_MIP_POINT;
};

SamplerState Sampler_Filter_Linear
{
	Filter = MIN_MAG_MIP_LINEAR;
};

SamplerState Sampler_Filter_ANISOTROPIC
{
	Filter = ANISOTROPIC;
};

struct VertexInput
{
	float4 Position : POSITION;
	float2 Uv : UV;
	//float2 TUv : UV1;
	float3 Normal : NORMAL;
};

struct VertexOutput
{
	float4 Position : SV_POSITION;
    float4 WPosition : POSITION1;
	float2 Uv : UV;
	//float2 TUv : UV1;
	float3 Normal : NORMAL;
};

struct BrushDesc
{
	float4 Color;
	float3 Location;
	uint Type;
	uint Range;
};

cbuffer Brush : register(b3)
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

cbuffer Line : register(b4)
{
	LineDesc Line;
}

Texture2D BaseMap : register(t0);
SamplerState BaseSamp : register(s0);

Texture2D LayerMap : register(t1);
SamplerState LayerSamp : register(s1);

Texture2D AlphaMap : register(t2);
SamplerState AlphaSamp : register(s2);

float4 GetBrushColor(float3 wPosition)
{
	//사각형
	[flatten]
    if (Brush.Type == 1)
    {
		[flatten]
        if ((wPosition.x >= (Brush.Location.x - Brush.Range)) &&
			(wPosition.x <= (Brush.Location.x + Brush.Range)) &&
			(wPosition.z >= (Brush.Location.z - Brush.Range)) &&
			(wPosition.z <= (Brush.Location.z + Brush.Range)))
        {
            return Brush.Color;
        }
    }
	//원
	[flatten]
    if (Brush.Type == 2)
    {
        float dx = (wPosition.x - Brush.Location.x);
        float dz = (wPosition.z - Brush.Location.z);
        float dist = sqrt(dx * dx + dz * dz);
		[flatten]
        if (dist <= (float) Brush.Range)
        {
            return Brush.Color;
        }
    }

    return float4(0, 0, 0, 0);
}

float4 GetLineColor(float3 wPosition)
{
	[flatten]
    if (Line.Visible < 1)
    {
        return float4(0, 0, 0, 0);
    }

    float2 grid = wPosition.xz / Line.Size;
    grid = frac(grid);

    float thick = Line.Thickness / Line.Size;

	[flatten]
    if (grid.x < thick || grid.y < thick)
    {
        return Line.Color;
    }

    return float4(0, 0, 0, 0);
}


VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.Position = WorldPosition(input.Position);
    output.WPosition = output.Position;
	output.Position = VPPosition(output.Position);
	//output.Position = input.Position;

	output.Uv = input.Uv;
	//output.TUv = input.TUv;
	output.Normal = mul(input.Normal, (float3x3)CB_World.World);

	return output;
}

float4 PS(VertexOutput input) : SV_TARGET
{
	float3 diffuse = BaseMap.Sample(terrainSample, input.Uv).rgb;
	float NdotL = dot(normalize(input.Normal), -CB_Light.Direction);

    float4 brush = GetBrushColor(input.WPosition.xyz);
    float4 lineColor = GetLineColor(input.WPosition.xyz);

    return float4(diffuse * NdotL, 1) + brush + lineColor;
}