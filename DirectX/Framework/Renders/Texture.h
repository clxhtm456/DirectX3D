#pragma once

#include <DirectXTex.h>

#pragma comment(lib,"DirectXTex.lib")

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

	D3D11_TEXTURE2D_DESC ReadPixel(DXGI_FORMAT readFormat, vector<Color>* pixels);
	static D3D11_TEXTURE2D_DESC ReadPixel(ID3D11Texture2D* src, DXGI_FORMAT readFormat, vector<Color>* pixels);

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

class TextureCube
{
public:
	TextureCube(UINT width, UINT height);
	~TextureCube();

	void Position(Vector3& position,Vector3& scale, float zNear = 0.1f,float zFar = 500.0f, float fov = 0.5f);
	void Set(Shader* shader);

	ID3D11ShaderResourceView* SRV() { return srv; }
	Perspective* GetPerspective() { return perspective; }

private:
	struct Desc
	{
		Matrix Views[6];
		Matrix Projection;
	}desc;

private:
	UINT width, height;

	ID3D11Texture2D* rtvTexture;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;

	ID3D11Texture2D* dsvTexture;
	ID3D11DepthStencilView* dsv;

	Perspective* perspective;
	Viewport* viewport;
};