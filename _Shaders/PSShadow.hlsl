struct PixelInput
{
    float4 Position : SV_POSITION0;
    float3 wPosition : POSITION1;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT;
};

cbuffer shadow : register(b2)
{
    Matrix View;
    Matrix Projection;

    float2 MapSize;
    float Bias;

    int Quality;
}

float4 PS(PixelInput input) : SV_TARGET0
{
	float4 Red = float4(1, 0, 0, 1);
	//float4 emissive = ComputeEmissive(input.Normal, input.wPosition);
	return Red;
}