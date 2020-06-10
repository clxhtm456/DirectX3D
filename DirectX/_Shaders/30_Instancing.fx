#include "00_Global.fx"
#include "00_Light.fx"

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;

    matrix Transform : InstTransform;
    uint InstanceID : SV_InstanceID;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    World = input.Transform;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3)World);
    output.Uv = input.Uv;

    return output;
}

float4 PS(VertexOutput input) : SV_Target0
{
    float3 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    float NdotL = dot(normalize(input.Normal), -GlobalLight.Direction);

    return float4(diffuse * NdotL, 1);
}


technique11 T0
{
    P_VP(P0, VS, PS)
}