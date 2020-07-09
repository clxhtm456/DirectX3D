#pragma once

class Shadow  : public AlignedAllocationPolicy<16>
{
public:
	Shadow(DirectionLight* light ,Vector3 position, float radius, UINT width = 1024, UINT height = 1024);
	~Shadow();

	void Set();
	void Draw();

	void SetShadowMap(ID3D11ShaderResourceView* srv);
private:
	void UpdateVolume();
private:
	struct VsDesc
	{
		Matrix View;
		Matrix Projection;
	}vsDesc;

	struct PsDesc
	{
		Vector2 MapSize;
		float Bias = -0.0006f;

		UINT Quality = 2;
		Vector4 testColor;
	}psDesc;
private:
	Shader * shader;
	UINT width, height;

	Vector3 position;
	float radius;

	DirectionLight* _light;

	ConstantBuffer* vsBuffer;
	ConstantBuffer* psBuffer;
	ID3D11ShaderResourceView* shadowMap;
	ID3D11SamplerState* shadowSample;
};