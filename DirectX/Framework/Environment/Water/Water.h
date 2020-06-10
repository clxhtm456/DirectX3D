#pragma once

class Water : public Renderer
{
public :
	Water(Shader* shader, float radius, UINT width = 0, UINT height = 0);
	~Water();

	void RestartClipPlane(float x, float y, float z, float w);

	void Update()override;
	void PreRender_Reflection();
	void PreRender_Refraction();
	void Render()override;
private:
	struct WaterDesc
	{
		Color ReflectionColor = Color(0.0f,0.0f,0.3f,1.0f);

		Vector2 NormalMapTile = Vector2(0.1f,0.2f);
		float WaveTranslation = 0.0f;
		float WaveScale = 0.05f;

		float WaterShiness = 20.0f;
		float WaterAlpha = 0.3f;
		float Padding[2];
	}waterDesc;
private:
	float radius;
	UINT width, height;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	float waveSpeed= 0.06f;
	Texture* normalMap;
	ID3DX11EffectShaderResourceVariable* sNormalMap;

	Camera* camera;
	RenderTarget* reflection;
	RenderTarget* refraction;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ID3DX11EffectMatrixVariable* sReflection;

	ID3DX11EffectShaderResourceVariable* sReflectionMap;
	ID3DX11EffectShaderResourceVariable* sRefractionMap;
};