#include "00_Global.fx"
#include "00_Light.fx"

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
};



VertexOutput VS(VertexTextrueNormal input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3)World);
    output.Uv = input.Uv;

    return output;
}

float4 PS(VertexOutput input) : SV_Target0
{
    float3 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv).rgb;

    float NdotL = dot(normalize(input.Normal), -GlobalLight.Direction);

    return float4(diffuse * NdotL, 1);
}


RasterizerState RS
{
    FillMode = WireFrame;
};


technique11 T0
{
    P_VP(P0, VS, PS)
    P_RS_VP(P1, RS, VS, PS)
}