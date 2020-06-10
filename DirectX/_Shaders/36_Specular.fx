#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Model.fx"

float4 PS(MeshOutput input) : SV_Target0
{
    Texture(Material.Diffuse, DiffuseMap, input.Uv); 
    Texture(Material.Specular, SpecularMap, input.Uv); 

    MaterialDesc result = MakeMaterial();
    ComputeLight(result, input.Normal, input.wPosition);

    return float4(MaterialToColor(result), 1.0f);

}

technique11 T0
{
    P_VP(P0, VS_Mesh, PS)
    P_VP(P1, VS_Model, PS)
    P_VP(P2, VS_Animation, PS)
}