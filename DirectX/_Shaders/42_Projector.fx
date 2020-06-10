#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Model.fx"

MeshOutput VS_Mesh_Projector(VertexMesh input)
{
    MeshOutput output = VS_Mesh(input);
    VS_Projector(output.wvpPosition_Sub, input.Position);

    return output;
}

MeshOutput VS_Model_Projector(VertexModel input)
{
    MeshOutput output = VS_Model(input);
    VS_Projector(output.wvpPosition_Sub, input.Position);

    return output;
}

MeshOutput VS_Animation_Projector(VertexModel input)
{
    MeshOutput output = VS_Animation(input);
    VS_Projector(output.wvpPosition_Sub, input.Position);

    return output;
}

float4 PS(MeshOutput input) : SV_Target0
{
    float4 color = PS_AllLight(input);

    PS_Projection(color, input.wvpPosition_Sub);

    return color;
}

technique11 T0
{
    P_VP(P0, VS_Mesh_Projector, PS)
    P_VP(P1, VS_Model_Projector, PS)
    P_VP(P2, VS_Animation_Projector, PS)
}