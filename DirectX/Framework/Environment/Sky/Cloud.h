#pragma once

class Cloud : public Renderer
{
public:
	Cloud(Shader* shader);
	~Cloud();

	void Update()override;
	void Render()override;

	void PostRender();
	void PreRender();

private:
	void CreateTexture();
private:
	ID3D11Texture2D * texture;
	ID3D11ShaderResourceView* srv;
	ID3DX11EffectShaderResourceVariable* sSrv;

	Render2D * render2D;
};