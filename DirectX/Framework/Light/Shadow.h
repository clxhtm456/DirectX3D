#pragma once

class Shadow
{
public:
	Shadow(DirectionLight* light ,Vector3 position, float radius, UINT width = 1024, UINT height = 1024);
	~Shadow();

	void Set();

	ID3D11ShaderResourceView* SRV() { return renderTarget->SRV(); }

private:
	void UpdateVolume();
private:
	struct Desc
	{
		Matrix View;
		Matrix Projection;

		Vector2 MapSize;
		float Bias = -0.0006f;

		UINT Quality = 2;
	}desc;
private:
	Shader * shader;
	UINT width, height;

	Vector3 position;
	float radius;

	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ConstantBuffer* buffer;
	ID3D11ShaderResourceView* shadowMap;
	ID3D11SamplerState* shadowSample;
};