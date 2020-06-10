//ByteAddressBuffer Input;
RWByteAddressBuffer Output;

struct CS_Input
{
    uint3 GroupID : SV_GroupID;
    uint3 GroupThreadID : SV_GroupThreadID;
    uint3 DispatchThreadID : SV_DispatchThreadID;
    uint GroupIndex : SV_GroupIndex;
};

[numthreads(10, 8, 3)]
void CS(CS_Input input)
{
    uint3 GroupID = asuint(input.GroupID);
    uint3 GroupThreadID = asuint(input.GroupThreadID);
    uint3 DispatchThreadID = asuint(input.DispatchThreadID);
    uint GroupIndex = asuint(input.GroupIndex);

    uint index = (input.GroupID * 240 + input.GroupIndex) * 10 * 4;

    Output.Store3(index + 0, asuint(GroupID));
    Output.Store3(index + 12, asuint(GroupThreadID));
    Output.Store3(index + 24, asuint(DispatchThreadID));
    Output.Store(index + 36, asuint(GroupIndex));

}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}