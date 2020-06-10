#include "00_Global.fx"
#include "00_Light.fx"

cbuffer CB_Snow
{
    float4 Color;

    float3 Velocity;
    float DrawDistance;

    float3 Origin;
    float Turbulence;

    float3 Extent;
};

struct VertexInput
{
    float4 Position : Position0;
    float2 Uv : Uv0;
    float Scale : Scale0;
    float2 Random : Random0;
};

struct VertexOutput
{
    float4 Positoin : SV_Position;
    float2 Uv : Uv0;
    float Alpha : Alpha0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    float3 displace = Time *Velocity;
   
    input.Position.y = Origin.y + Extent.y - (input.Position.y - displace.y) % Extent.y;
    input.Position.x += cos(Time - input.Random.x) * Turbulence;
    input.Position.z += cos(Time - input.Random.y) * Turbulence;


    input.Position.xyz = Origin + (Extent + (input.Position.xyz + displace) % Extent) % Extent - (Extent * 0.5f);

    float4 position = WorldPosition(input.Position);

    float3 up = normalize(-Velocity);
    float3 forward = position.xyz - ViewPosition();
    float3 right = normalize(cross(up, forward));

    position.xyz += (input.Uv.x - 0.5f) * right * input.Scale;
    position.xyz += (1.0f - input.Uv.y - 0.5f) * up * input.Scale;
    position.w = 1.0f;

    output.Positoin = ViewProjection(position);
    output.Uv = input.Uv;

    float4 view = mul(position, View);
   
    output.Alpha = saturate(1 - view.z / DrawDistance) * 0.5f;

    return output;
}

float4 PS(VertexOutput input) : SV_Target0
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);  

    color.rgb = Color.rgb * input.Alpha * 2.0f;
    color.a = color.a * (input.Alpha * 1.5f);

    return color;
}

technique11 T0
{
    P_BS_VP(P0, AlphaBlend_AlphaToCoverage, VS, PS)
}