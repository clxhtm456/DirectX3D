#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Model.fx"


float4 PS(MeshOutput input) : SV_Target0
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);

    clip(diffuse.a - 0.5f);

    float NdotL = dot(normalize(input.Normal), -GlobalLight.Direction);

    return float4(diffuse.rgb * NdotL, 1);
}

technique11 T0
{
    P_VP(P0, VS_Mesh, PS)    
}