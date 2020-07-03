#include "Framework.h"
#include "Texture.h"



map<wstring, ID3D11ShaderResourceView*> Texture::totalSRV;
vector<Texture*> Texture::totalTexture;

Texture::Texture(ID3D11ShaderResourceView* SRV, wstring file)
	: SRV(SRV), file(file)
{

	SRV->GetResource((ID3D11Resource**)&srcTexture);
	srcTexture->GetDesc(&srcDesc);

	width = srcDesc.Width;
	height = srcDesc.Height;

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = D3D::GetDevice()->CreateSamplerState(&sampDesc, &sampler);
	Check(hr);
}

Texture::~Texture()
{
	sampler->Release();
}

ScratchImage Texture::LoadTextureFromFile(wstring file)
{
	// Load the texture.
	string str = String::ToString(file);
	wstring wsTmp(str.begin(), str.end());

	wstring ws = wsTmp;
	// Load the texture.
	WCHAR ext[_MAX_EXT];
	_wsplitpath_s(ws.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

	HRESULT hr;
	ScratchImage image;
	if (_wcsicmp(ext, L".dds") == 0)
	{
		hr = LoadFromDDSFile(ws.c_str(), DDS_FLAGS_NONE, nullptr, image);
	}

	else if (_wcsicmp(ext, L".tga") == 0)
	{
		hr = LoadFromTGAFile(ws.c_str(), nullptr, image);
	}

	else
	{
		hr = LoadFromWICFile(ws.c_str(), WIC_FLAGS_NONE, nullptr, image);
	}

	return image;
}

Texture* Texture::Add(wstring file)
{

	file = L"../../_Textures/" + file;

	return AddAbsPath(file);
}

Texture* Texture::AddAbsPath(wstring file)
{
	if (!Path::ExistFile(String::ToString(file)))
	{
		file = L"../../_Textures/White.png";
	}

	totalTexture.push_back(new Texture(LoadSRV(file), file));

	return totalTexture.back();
}

ID3D11ShaderResourceView* Texture::LoadSRV(wstring file)
{
	ID3D11ShaderResourceView* SRV;

	if (totalSRV.count(file) > 0)
	{
		SRV = totalSRV[file];
	}
	else
	{
		DirectX::ScratchImage image = LoadTextureFromFile(file.c_str());
		HRESULT hr = CreateShaderResourceView(D3D::GetDevice(), image.GetImages(), image.GetImageCount(), image.GetMetadata(), &SRV);

		Check(hr);

		//totalSRV[file] = SRV;
		//totalSRV.insert(pair<wstring, ID3D11ShaderResourceView*>(file, SRV));
		//totalSRV.insert(make_pair(file, SRV));
		totalSRV.insert({ file, SRV });
	}
	return SRV;
}

void Texture::Delete()
{
	for (auto SRV : totalSRV)
		SRV.second->Release();

	for (Texture* texture : totalTexture)
		delete texture;
}

void Texture::DeleteOne(Texture * texture)
{
	for (auto iter = totalSRV.begin(); iter != totalSRV.end(); iter++)
	{
		if ((*iter).second == texture->GetSRV())
		{
			(*iter).second->Release();
			totalSRV.erase(iter);
			break;
		}
	}

	for (auto iter = totalTexture.begin(); iter != totalTexture.end(); iter++)
	{
		if (*iter == texture)
		{
			totalTexture.erase(iter);
			delete texture;
			break;
		}
	}
}

vector<Vector4> Texture::ReadPixels()
{
	vector<Vector4> pixels;

	D3D11_TEXTURE2D_DESC destDesc = {};
	destDesc.Width = width;
	destDesc.Height = height;
	destDesc.MipLevels = 1;
	destDesc.ArraySize = 1;
	destDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	destDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	destDesc.Usage = D3D11_USAGE_STAGING;
	destDesc.SampleDesc = srcDesc.SampleDesc;
	
	ID3D11Texture2D* destTexture;
	D3D::GetDevice()->CreateTexture2D(&destDesc, nullptr, &destTexture);

	D3D::GetDC()->CopySubresourceRegion(destTexture, 0, 0, 0, 0, srcTexture, 0, nullptr);

	UINT* colors = new UINT[width * height];
	D3D11_MAPPED_SUBRESOURCE map;

	D3D::GetDC()->Map(destTexture, 0, D3D11_MAP_READ, 0, &map);
	memcpy(colors, map.pData, sizeof(UINT) * width * height);
	D3D::GetDC()->Unmap(destTexture, 0);

	for (UINT i = 0; i < width * height; i++)
	{
		float f = 1.0f / 255.0f;

		float a = f * (float)((0xff000000 & colors[i]) >> 24);
		float b = f * (float)((0x00ff0000 & colors[i]) >> 16);
		float g = f * (float)((0x0000ff00 & colors[i]) >> 8);
		float r = f * (float)((0x000000ff & colors[i]) >> 0);

		pixels.push_back(Vector4(r, g, b, a));
	}

	destTexture->Release();
	delete[] colors;

	return pixels;
}

void Texture::Set(UINT slot)
{
	D3D::GetDC()->PSSetShaderResources(slot, 1, &SRV);
	D3D::GetDC()->PSSetSamplers(slot, 1, &sampler);
}
