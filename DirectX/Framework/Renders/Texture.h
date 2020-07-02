#pragma once

#include "DirectXTex.h"

#pragma comment(lib, "directxtex.lib")

class Texture
{
private:
	wstring file;

	ID3D11ShaderResourceView* SRV;
	ID3D11SamplerState* sampler;

	ID3D11Texture2D* srcTexture;
	D3D11_TEXTURE2D_DESC srcDesc;

	UINT width, height;


	static map<wstring, ID3D11ShaderResourceView*> totalSRV;
	static vector<Texture*> totalTexture;

	Texture(ID3D11ShaderResourceView* SRV, wstring file);	
	~Texture();

	static ScratchImage LoadTextureFromFile(wstring file);
public:
	static Texture* Add(wstring file);
	static Texture* AddAbsPath(wstring file);
	static ID3D11ShaderResourceView* LoadSRV(wstring file);
	static void Delete();
	static void DeleteOne(Texture* texture);

	vector<Vector4> ReadPixels();

	void Set(UINT slot);

	ID3D11ShaderResourceView* GetSRV() { return SRV; }
	UINT GetWidth() { return width; }
	UINT GetHeight() { return height; }

	wstring GetFile() { return file; }
};