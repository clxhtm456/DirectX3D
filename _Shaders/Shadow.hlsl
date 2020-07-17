Texture2D shadowMap : register(t10);
SamplerComparisonState ShadowSampler : register(s10);

SamplerState LinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = Wrap;
	AddressV = Wrap;
};


cbuffer CB_ShadowVS : register(b10)
{
	matrix ShadowView;
	matrix ShadowProjection;
};

cbuffer CB_ShadowPS : register(b10)
{
	float2 MapSize;
	float Bias;

	uint Quality;
	float4 testColor;
};



struct ShadowPixelInput
{
	float4 Position : SV_POSITION;
	float4 wPosition : POSITION1;
	float4 sPosition : POSITION2;//shadow position
	float2 Uv : UV;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
	float3 ViewPos : VIEWPOS;
};

float4 PS_Shadow(ShadowPixelInput input, float4 color) : SV_Target0
{
	input.sPosition.xyz /= input.sPosition.w;

	[flatten]
	if (input.sPosition.x < -1.0f || input.sPosition.x > 1.0f ||
		input.sPosition.y < -1.0f || input.sPosition.y > 1.0f ||
		input.sPosition.z < 0.0f || input.sPosition.z > 1.0f)
		return color;

	input.sPosition.x = input.sPosition.x * 0.5f + 0.5f;
	input.sPosition.y = -input.sPosition.y * 0.5f + 0.5f;
	input.sPosition.z -= Bias;//z축 직접수정을 통한 피터패닝 수정가능(권장하진않음)

	float depth = 0.0f;
	float factor = 0.0f;

	[branch]
	if (Quality == 0)
	{
		depth = shadowMap.Sample(LinearSampler, input.sPosition.xy).r;
		factor = (float)input.sPosition.z <= depth;
	}
	else if (Quality == 1)
	{
		depth = input.sPosition.z;
		factor = shadowMap.SampleCmpLevelZero(ShadowSampler, input.sPosition.xy, depth).r;
	}
	else if (Quality == 2)
	{
		depth = input.sPosition.z;
		float2 size = 1.0f / MapSize;//그림자맵 한픽셀의 사이즈
		float2 offsets[] =
		{
			float2(-size.x, -size.y), float2(0.0f, -size.y),
			float2(size.x, -size.y), float2(-size.x, 0.0f),
			float2(0.0f, 0.0f), float2(+size.x, 0.0f),
			float2(-size.x, +size.y), float2(0.0f, +size.y),
			float2(size.x, size.y)
		};

		float2 uv = 0;
		float2 sum = 0;

		[unroll(9)]
		for (int i = 0; i < 9; i++)
		{
			uv = input.sPosition.xy + offsets[i];
			sum += shadowMap.SampleCmpLevelZero(ShadowSampler, uv, depth).r;
		}

		factor = sum / 9.0f;
	}

	factor = saturate(factor + depth);

	return float4(color.rgb * factor ,1);
	//return float4(1, 0, 0 ,1);
	
}