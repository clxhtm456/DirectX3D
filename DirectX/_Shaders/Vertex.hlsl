#include "Header.hlsli"

float4 VS(float4 Pos : Position) : SV_Position
{
    return Pos;
}

float4 PS(float4 input : SV_Position) : SV_Target
{
    return float4(1, 0, 0, 1);
}