#include "00_Global.fx"
#include "00_Deffered.fx"
#include "00_Model.fx"



/////////////////////////////////////////////////////////////////////////////////////////////////



technique11 T0
{
    //Sky
    //P_RS_DSS_VP(P0,FrontCounterClockwise_True,DepthEnable_False,VS_Mesh,PS_Sky)

    //Deffered
    P_DSS_VP(P0, Deffered_DepthStencil_State, VS_Mesh, PS_PackGBuffer)
    P_DSS_VP(P1, Deffered_DepthStencil_State, VS_Model, PS_PackGBuffer)
    P_DSS_VP(P2, Deffered_DepthStencil_State, VS_Animation, PS_PackGBuffer)

    //Deffered-Directional!
    P_DSS_VP(P3, Deffered_DepthStencil_State, VS_Directional, PS_Directional)

    //Deffered_PointLights
    P_RS_VTP(P4, Deffered_Rasterizer_State, VS_PointLights, HS_PointLights, DS_PointLights, PS_PointLight_Debug)
    P_RS_DSS_BS_VTP(P5,Deffered_Rasterizer_State,Deffered_DepthStencil_State,Blend_Addtive,VS_PointLights,HS_PointLights,DS_PointLights,PS_PointLights)
}