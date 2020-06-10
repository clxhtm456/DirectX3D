#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Model.fx"

float4 PS(MeshOutput input) : SV_Target
{
    return PS_Shadow(input, PS_AllLight(input));

}

technique11 T0
{
    P_RS_VP(P0, FrontCounterClockwise_True,VS_Depth_Mesh, PS_Depth)
    P_RS_VP(P1, FrontCounterClockwise_True,VS_Depth_Model, PS_Depth)
    P_RS_VP(P2, FrontCounterClockwise_True,VS_Depth_Animation, PS_Depth)

    P_VP(P3, VS_Mesh, PS)
    P_VP(P4, VS_Model, PS)
    P_VP(P5, VS_Animation, PS)
}