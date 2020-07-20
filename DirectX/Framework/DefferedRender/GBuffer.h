#pragma once

class GBuffer
{
public:
	GBuffer(UINT width = 0, UINT height = 0);
	~GBuffer();

	void PackGBuffer();
	void Render(Camera* viewer);
	void DebugRender(Camera* viewer);

private:
	void CreateDepthStencilView();
	void CreateDepthStencilState();
	void CreateRasterizerState();
private:
	void RenderDirectional();
	void CalcPointLights(UINT count);
	void RenderPointLights();

	void CalcSpotLights(UINT count);
	void RenderSpotLights();
public:
	Shader* GetShader() { return deffredShader; }
	void Resize(float width, float height);
	//void SetDebug(bool val) { bDebug = val; }
private:
	/*struct Desc
	{
		Vector4 Perspective;
		Vector2 PowerRange = Vector2(1e-6f, 15.0f);
		float Padding[2];
	} desc;

	struct PointLightDesc
	{
		float TessFactor = 16.0f;
		float Padding[3];

		Matrix Projection[MAX_POINT_LIGHT];
		PointLight PointLight[MAX_POINT_LIGHT];
	}pointLightDesc;

	struct SpotLightDesc
	{
		float TessFactor = 16.0f;
		float Padding[3];

		Vector4 Angle[MAX_SPOT_LIGHT];
		Matrix Projection[MAX_POINT_LIGHT];
		SpotLight SpotLight[MAX_POINT_LIGHT];
	}spotLightDesc;*/
public:
	void SetDrawPointLights(bool val) { bDrawPointLights = val; }
	void SetDrawSpotLights(bool val) { bDrawSpotLights = val; }
private:
	bool bDrawPointLights = true;
	bool bDrawSpotLights = true;

private:
	Shader * deffredShader;
	Shader * gbufferShader;
	UINT width, height;

	RenderTarget* diffuseRTV;
	RenderTarget* specularRTV;
	RenderTarget* emissiveRTV;
	RenderTarget* normalRTV;
	RenderTarget* tangentRTV;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ID3D11DepthStencilView* depthStencilReadOnly;

	DepthStencilState* packDss;
	DepthStencilState* noDepthWriteLessDSS;
	DepthStencilState* noDepthWriteGreaterDSS;

	ID3D11RasterizerState* debugRSS;
	ID3D11RasterizerState* lightRSS;
	RasterizerState* RSS;
	//ID3DX11EffectRasterizerVariable* sRSS;

	//ID3DX11EffectShaderResourceVariable* sSrvs;

	ConstantBuffer* pointLightBuffer;
	//ID3DX11EffectConstantBuffer* sPointLightBuffer;

	ConstantBuffer* spotLightBuffer;
	//ID3DX11EffectConstantBuffer* sSpotLightBuffer;

	class Render2D* debug2D[6];

	ID3D11ShaderResourceView* srv;
};
