#pragma once

class Dome : public Renderer
{
public : 
	Dome(Shader* shader, Vector3 position , Vector3 scale, UINT drawCount = 32);
	~Dome();

	void Update() override;
	void Render() override;

private:
	Texture* starMap;

	ID3DX11EffectShaderResourceVariable* sStarMap;

	ID3D11DepthStencilState* dss;
	ID3DX11EffectDepthStencilVariable* sDss;
};