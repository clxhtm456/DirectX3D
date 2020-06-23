#pragma once

#include <DirectXTex.h>

#pragma comment(lib,"DirectXTex.lib")

//fbxsdk
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")

class Texture
{
public:
	friend class Textures;
private:
	/*typedef struct _D3DX11_TEXTURE_LOAD_INFO {
		D3D11_BOX *pSrcBox;
		D3D11_BOX *pDstBox;
		UINT SrcFirstMip;
		UINT DstFirstMip;
		UINT NumMips;
		UINT SrcFirstElement;
		UINT DstFirstElement;
		UINT NumElements;
		UINT Filter;
		UINT MipFilter;
	} D3DX11_TEXTURE_LOAD_INFO;*/

public:

	/*HRESULT LoadTextureFromTexture(
		ID3D11DeviceContext       *pContext,
		ID3D11Resource            *pSrcTexture,
		D3DX11_TEXTURE_LOAD_INFO  *pLoadInfo,
		ID3D11Resource            *pDstTexture
	);*/
public:
	Texture(wstring file);
	~Texture();
	
	operator ID3D11ShaderResourceView*(){ return view; }


	wstring GetFile() { return file; }

	UINT GetWidth() { return metaData.width; }
	UINT GetHeight() { return metaData.height; }

	void GetImageInfo(TexMetadata* data)
	{
		*data = metaData;
	}

	ID3D11ShaderResourceView* SRV() { return view; }
	ID3D11Texture2D* GetTexture();

private:
	wstring file;

	TexMetadata metaData;
	ID3D11ShaderResourceView* view;
};

struct TextureDesc
{
	wstring file;
	UINT width, height;
	TexMetadata metaData;
	ID3D11ShaderResourceView* view;

	bool operator==(const TextureDesc& desc)
	{
		bool b = true;
		b &= file == desc.file;
		b &= width == desc.width;
		b &= height == desc.height;

		return b;
	}
};

class Textures
{
public:
	friend class Texture;

public:
	static void Create();
	static void Delete();

private:
	static void Load(Texture* texture);

private:
	static vector<TextureDesc> descs;
};

class TextureArray
{
public:
	TextureArray(vector<wstring>& names, UINT width = 256, UINT height = 256, UINT mipLevels = 1);
	~TextureArray();

	ID3D11ShaderResourceView* SRV() { return srv; }

private:
	vector<ID3D11Texture2D *> CreateTextures(vector<wstring>& names, UINT width, UINT height, UINT mipLevels);

private:
	ID3D11ShaderResourceView* srv;
};