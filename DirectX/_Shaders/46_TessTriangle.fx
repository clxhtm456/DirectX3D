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
    float Edge[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};
float4 Factor;
ConstantHullOutput HS_Constant(InputPatch<VertexOutput,3> input)
{
    ConstantHullOutput output;

    output.Edge[0] = Factor.x;
    output.Edge[1] = Factor.y;
    output.Edge[2] = Factor.z;
    output.Inside = Factor.w;

    return output;
}

struct HullOutput
{
    float4 Position : Position;
};

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HS_Constant")]
HullOutput HS(InputPatch<VertexOutput,3> input,uint pointID : SV_OutputControlPointID)
{
    HullOutput output;
    output.Position = input[pointID].Position;

    return output;
}

struct DomainOutput
{
    float4 Position : SV_Position;
};

[domain("tri")]
DomainOutput DS(ConstantHullOutput input,const OutputPatch<HullOutput,3> patch, float3 uvw : SV_DomainLocation)
{
    DomainOutput output;

    float3 position = uvw.x * patch[0].Position.xyz + uvw.y * patch[1].Position.xyz + uvw.z * patch[2].Position.xyz;
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