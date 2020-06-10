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

float2 Distance;
ConstantHullOutput HS_Constant(InputPatch<VertexOutput,4> input)
{
    ConstantHullOutput output;

    float4 position = 0;

    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        position.xyz == input[i].Position.xyz;
    }
    float3 center = position.xyz * 0.25f;

    position = mul(float4(center, 1), ViewPosition());
    float4 dist = distance(position.xyz, ViewPosition());
    float factor = saturate((Distance.y - dist) / (Distance.y- Distance.x))*64;

    output.Edge[0] = factor;
    output.Edge[1] = factor;
    output.Edge[2] = factor;
    output.Edge[3] = factor;

    output.Inside[0] = factor;
    output.Inside[1] = factor;

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
HullOutput HS_Integer(InputPatch<VertexOutput,4> input,uint pointID : SV_OutputControlPointID)
{
    HullOutput output;
    output.Position = input[pointID].Position;

    return output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HS_Constant")]
HullOutput HS_Odd(InputPatch<VertexOutput, 4> input, uint pointID : SV_OutputControlPointID)
{
    HullOutput output;
    output.Position = input[pointID].Position;

    return output;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HS_Constant")]
HullOutput HS_Even(InputPatch<VertexOutput, 4> input, uint pointID : SV_OutputControlPointID)
{
    HullOutput output;
    output.Position = input[pointID].Position;

    return output;
}


[domain("quad")]
[partitioning("pow2")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HS_Constant")]
HullOutput HS_Pow2(InputPatch<VertexOutput, 4> input, uint pointID : SV_OutputControlPointID)
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
    output.Position = WorldPosition(output.Position);
    output.Position = ViewProjection(output.Position);

    return output;
}

float4 PS(DomainOutput input) : SV_Target0
{
    return float4(0, 1, 0, 1);
}

technique11 T0
{
    P_RS_VTP(P0, FillMode_WireFrame, VS, HS_Integer,DS, PS)
    P_RS_VTP(P1, FillMode_WireFrame, VS, HS_Odd, DS, PS)
    P_RS_VTP(P2, FillMode_WireFrame, VS, HS_Even, DS, PS)
    P_RS_VTP(P3, FillMode_WireFrame, VS, HS_Pow2, DS, PS)
}