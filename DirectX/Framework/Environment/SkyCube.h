#pragma once

class SkyCube
{
public:
	SkyCube(wstring file, Shader* shader = nullptr);
	~SkyCube();

	void Update();
	void Render();

	void Pass(UINT val) { pass = val; sphereRender->Pass(pass);
	}

private:
	Shader * shader;
	MeshRender* sphereRender;

	bool bCreateShader = false;
	UINT pass;

	ID3D11ShaderResourceView* srv;
	ID3DX11EffectShaderResourceVariable* sSrv;

	ID3D11DepthStencilState* dss;
	ID3DX11EffectDepthStencilVariable* sDss;
};