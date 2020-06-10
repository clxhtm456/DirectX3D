#include "00_Global.fx"
#include "00_Deffered.fx"
#include "00_Model.fx"
#include "00_Sky.fx"



/////////////////////////////////////////////////////////////////////////////////////////////////



technique11 T0
{
    //Deffered - Depth
    P_RS_VP(P0,Deffered_Rasterizer_State,VS_Depth_Mesh,PS_Depth)
    P_RS_VP(P1, Deffered_Rasterizer_State, VS_Depth_Model, PS_Depth)
    P_RS_VP(P2, Deffered_Rasterizer_State, VS_Depth_Animation, PS_Depth)

    //Deffered
    P_DSS_VP(P3, Deffered_DepthStencil_State, VS_Mesh, PS_PackGBuffer)
    P_DSS_VP(P4, Deffered_DepthStencil_State, VS_Model, PS_PackGBuffer)
    P_DSS_VP(P5, Deffered_DepthStencil_State, VS_Animation, PS_PackGBuffer)

    //Deffered-Directional!
    P_DSS_VP(P6, Deffered_DepthStencil_State, VS_Directional, PS_Directional)

    //Deffered_PointLights
    P_RS_VTP(P7, Deffered_Rasterizer_State, VS_PointLights, HS_PointLights, DS_PointLights, PS_PointLight_Debug)
    P_RS_DSS_BS_VTP(P8,Deffered_Rasterizer_State,Deffered_DepthStencil_State,Blend_Addtive,VS_PointLights,HS_PointLights,DS_PointLights,PS_PointLights)

//Deffered_SpotLights
    P_RS_VTP(P9, Deffered_Rasterizer_State, VS_SpotLights, HS_SpotLights, DS_SpotLights, PS_SpotLight_Debug)
    P_RS_DSS_BS_VTP(P10, Deffered_Rasterizer_State, Deffered_DepthStencil_State, Blend_Addtive, VS_SpotLights, HS_SpotLights, DS_SpotLights, PS_SpotLights)

//Sky
   // P_RS_DSS_VP(P11, FrontCounterClockwise_True, SkyDepthStencil, VS_Mesh, PS_Sky)
    P_VP(P11, VS_Scattering, PS_Scattering)
    P_DSS_VP(P12,SkyDepthStencil,VS_Dome, PS_Dome)
    P_BS_VP(P13, AlphaBlend, VS_Moon, PS_Moon)
    P_BS_VP(P14, AlphaBlend, VS_Cloud, PS_Cloud)
}