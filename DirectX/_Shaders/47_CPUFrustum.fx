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

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
    output.Normal = WorldNormal(input.Normal);

    output.Uv = input.Uv;

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);

    float3 normal = normalize(input.Normal);
    float NdotL = dot(normal, -GlobalLight.Direction);

    return diffuse * NdotL;
}

technique11 T0
{
    P_VP(P0,VS,PS)
    P_RS_VP(P1,FillMode_WireFrame,VS,PS)
}