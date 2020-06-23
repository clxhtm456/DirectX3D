#include "Header.hlsli"

struct VertexInput
{
	float4 Position : Position;
	float4 Color : Color;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Color : Color;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    //output.Position = WorldPosition(input.Postion);
    output.Position = ViewProjection(input.Position);
    output.Color = input.Color;

    return output;
}

float4 PS(VertexOutput input) : SV_Target0
{
    return input.Color;
}