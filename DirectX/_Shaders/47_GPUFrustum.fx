#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Model.fx"

struct VertexOutput
{
    float4 Position : SV_Position0; // Rasterizing
    float4 wvpPosition : Position1; //wvp position
    float4 wvpPosition_Sub : Position2; //sub wvp position(projection texture)
    float3 oPosition : Position3; //original
    float3 wPosition : Position4; //world position
    float4 sPosition : Position5; //shadow position
    float4 gPosition : Position6; //GeometryPosition;

    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;

    float4 Cull : SV_CullDistance0; //정점단위계산
    float4 Cull2 : SV_CullDistance1;

    //float4 Cull : SV_ClipDistance0;//픽셀단위 계산
    //float4 Cull2 : SV_ClipDistance1;

};

float4 Planes[6];
VertexOutput VS(VertexModel input)
{
    VertexOutput output;
    SetModelWorld(World, input);
    VS_GENERATE

    output.Cull.x = dot(float4(output.wPosition, 1), Planes[0]);
    output.Cull.y = dot(float4(output.wPosition, 1), Planes[1]);
    output.Cull.z = dot(float4(output.wPosition, 1), Planes[2]);
    output.Cull.w = dot(float4(output.wPosition, 1), Planes[3]);
    output.Cull2.x = dot(float4(output.wPosition, 1), Planes[4]);
    output.Cull2.y = dot(float4(output.wPosition, 1), Planes[5]);
    output.Cull2.z = 0.0f;
    output.Cull2.w = 0.0f;

    return output;
}

float4 PS(VertexOutput input) : SV_Target0
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);

    float3 normal = normalize(input.Normal);
    float NdotL = dot(normal, -GlobalLight.Direction);

    return diffuse * NdotL;
}

technique11 T0
{
    P_VP(P0, VS,PS)
    P_RS_VP(P1,FillMode_WireFrame,VS,PS)
}
