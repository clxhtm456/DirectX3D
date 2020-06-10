#pragma once

class PerFrame
{
public:
	PerFrame(Shader* shader);
	~PerFrame();

public:
	void Update();
	void Render();

	void Clipping(Plane& plane) { bufferDesc.Cliping = plane; }

private:
	struct BufferDesc
	{
		Matrix View;
		Matrix ViewInverse;
		Matrix Projection;
		Matrix ProjectionInverse;
		Matrix VP;

		Plane Culling[4];
		Plane Cliping;

		float Time;
		float Padding[3];
	}bufferDesc;

	struct FogDesc
	{
		Color FogColor;
		Vector2 FogDistance;
		float FogDensity;
		UINT FogType;
	}fogDesc;

	struct LightDesc
	{
		Color Ambient;
		Color Specular;
		Vector3 Direction;
		float Padding;

		Vector3 Position;
		float Padding2;
	}lightDesc;

	struct PointLightDesc
	{
		UINT Count = 0;
		float Padding[3];

		PointLight Lights[MAX_POINT_LIGHT];
	} pointLightDesc;

	struct SpotLightDesc
	{
		UINT Count = 0;
		float Padding[3];

		SpotLight Lights[MAX_SPOT_LIGHT];
	} spotLightDesc;

private:
	Shader * shader;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	ConstantBuffer* lightBuffer;
	ID3DX11EffectConstantBuffer* sLightBuffer;

	ConstantBuffer* pointLightBuffer;
	ID3DX11EffectConstantBuffer* sPointLightBuffer;

	ConstantBuffer* spotLightBuffer;
	ID3DX11EffectConstantBuffer* sSpotLightBuffer;

	ConstantBuffer* fogBuffer;
	ID3DX11EffectConstantBuffer* sFogBuffer;

	ConstantBuffer* reflectionBuffer;
	ID3DX11EffectConstantBuffer* sReflectionBuffer;
};