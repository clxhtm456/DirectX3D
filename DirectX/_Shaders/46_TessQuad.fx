#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Model.fx"



struct VertexOutput
{
    float4 Position : Position;
};

VertexOutput VS(Vertex input)
{
    VertexOutput output;
    output.Position = input.Position;

    return output;
}

struct ConstantHullOutput
{
    float Edge[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

float4 Factor;
float2 Inside;
ConstantHullOutput HS_Constant(InputPatch<VertexOutput,4> input)
{
    ConstantHullOutput output;

    output.Edge[0] = Factor.x;
    output.Edge[1] = Factor.y;
    output.Edge[2] = Factor.z;
    output.Edge[3] = Factor.w;
    output.Inside[0] = Inside.x;
    output.Inside[1] = Inside.y;

    return output;
}

struct HullOutput
{
    float4 Position : Position;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HS_Constant")]
HullOutput HS(InputPatch<VertexOutput,4> input,uint pointID : SV_OutputControlPointID)
{
    HullOutput output;
    output.Position = input[pointID].Position;

    return output;
}

struct DomainOutput
{
    float4 Position : SV_Position;
};

[domain("quad")]
DomainOutput DS(ConstantHullOutput input,const OutputPatch<HullOutput,4> patch, float2 uv : SV_DomainLocation)
{
    DomainOutput output;

    //Bezier curve
    float3 v1 = lerp(patch[0].Position.xyz, patch[1].Position.xyz, 1 - uv.y);
    float3 v2 = lerp(patch[2].Position.xyz, patch[3].Position.xyz, 1 - uv.y);
    float3 position = lerp(v1, v2, uv.x);

    output.Position = float4(position, 1);

    return output;
}

float4 PS(DomainOutput input) : SV_Target0
{
    return float4(0, 1, 0, 1);
}

technique11 T0
{
    P_RS_VTP(P0, FillMode_WireFrame, VS, HS,DS, PS)
}