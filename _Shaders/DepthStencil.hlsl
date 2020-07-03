cbuffer VPBuffer : register(b2)
{
	matrix DepthView;
	matrix DepthProjection;
}
cbuffer WorldBuffer : register(b1)
{
	matrix DepthWorld;
}

struct VertexInput
{
	float4 Position : POSITION;
};

struct PixelInput
{
	float4 Position : SV_POSITION;
	float4 DepthPosition : TEXTURE0;
};


PixelInput VS(VertexInput input)
{
	PixelInput output;

	input.Position.w = 1.0f;

	output.Position = mul(input.Position, DepthWorld);
	output.Position = mul(output.Position, DepthView);
	output.Position = mul(output.Position, DepthProjection);

	output.DepthPosition = output.Position;

	return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
	float depthValue;
	float4 color;

	depthValue = input.DepthPosition.z / input.DepthPosition.w;

	if (depthValue < 0.9f)
	{
		color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	if (depthValue > 0.9f)
	{
		color = float4(0.0f, 1.0f, 0.0f, 1.0f);
	}

	if (depthValue > 0.925f)
	{
		color = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	return color;
}