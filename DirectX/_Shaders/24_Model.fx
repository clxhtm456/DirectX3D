#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Model.fx"


float4 PS(MeshOutput input) : SV_Target0
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
    P_VP(P0, VS_Model, PS)
    P_RS_VP(P1, RS, VS_Model, PS)
}