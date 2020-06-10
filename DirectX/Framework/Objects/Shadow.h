#pragma once

class Shadow
{
public:
	Shadow(Shader* shader, Vector3 position, float radius, UINT width = 1024, UINT height = 1024);
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
	ID3DX11EffectConstantBuffer* sBuffer;
	ID3DX11EffectShaderResourceVariable* sShadowMap;

	ID3D11SamplerState* samplerState;
	ID3DX11EffectSamplerVariable* sSamplerState;
};