#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Model.fx"

technique11 T0
{
    P_RS_DSS_VP(P0, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS_Sky)
}
