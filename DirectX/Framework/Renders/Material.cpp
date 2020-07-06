#include "Framework.h"
#include "Material.h"


Material::Material()
{
	materialBuffer = new MaterialBuffer();
	Initialize();


	SetDiffuseMap("../../_Textures/White.png");
	SetSpecularMap("../../_Textures/White_Specular.png");
	SetNormalMap("../../_Textures/White_Normal.png");
}

Material::Material(wstring diffuseMap, wstring specularMap, wstring normalMap)
{
	Initialize();

	SetDiffuseMap(diffuseMap);
	SetSpecularMap(specularMap);
	SetNormalMap(normalMap);
}

void Material::Initialize()
{
	diffuseMap = nullptr;
	specularMap = nullptr;
	normalMap = nullptr;

	
}

Material::~Material()
{
	delete materialBuffer;
}

void Material::SetName(wstring name)
{
	this->name = name;
}

void Material::SetAmbient(Color color)
{
	materialBuffer->data.ambient = color;
}

void Material::SetAmbient(float r, float g, float b, float a)
{
	Color temp = Color(r, g, b, a);
	SetAmbient(temp);
}

void Material::SetDiffuse(Color color)
{
	materialBuffer->data.diffuse = color;
}

void Material::SetDiffuse(float r, float g, float b, float a)
{
	Color temp = Color(r, g, b, a);
	SetDiffuse(temp);
}

void Material::SetSpecular(Color color)
{
	materialBuffer->data.specular = color;
}

void Material::SetSpecular(float r, float g, float b, float a)
{
	Color temp = Color(r, g, b, a);
	SetSpecular(temp);
}

void Material::SetEmissive(Color color)
{
	materialBuffer->data.emissive = color;
}

void Material::SetEmissive(float r, float g, float b, float a)
{
	Color temp = Color(r, g, b, a);
	SetEmissive(temp);
}


void Material::SetDiffuseMap(string file)
{
	SetDiffuseMap(String::ToWString(file));
}

void Material::SetDiffuseMap(wstring file)
{
	diffuseMap = Texture::AddAbsPath(file);
}

void Material::SetSpecularMap(string file)
{
	SetSpecularMap(String::ToWString(file));
}

void Material::SetSpecularMap(wstring file)
{
	specularMap = Texture::AddAbsPath(file);
}

void Material::SetNormalMap(string file)
{
	SetNormalMap(String::ToWString(file));
}

void Material::SetNormalMap(wstring file)
{
	normalMap = Texture::AddAbsPath(file);
}

void Material::Render()
{
	materialBuffer->SetPSBuffer(PS_MATERIALBUFFER);

	if (diffuseMap != nullptr)
	{
		diffuseMap->Set(0);
	}
	/*else
	{
		ID3D11ShaderResourceView* const pSRV[1] = { NULL };
		D3D::GetDC()->PSSetShaderResources(0, 1, pSRV);
	}*/
		
	if (specularMap != nullptr)
	{
		specularMap->Set(1);
	}
	/*else
	{
		ID3D11ShaderResourceView* const pSRV[1] = { NULL };
		D3D::GetDC()->PSSetShaderResources(0, 1, pSRV);
	}*/

	if (normalMap != nullptr)
	{
		normalMap->Set(2);
	}
	/*else
	{
		ID3D11ShaderResourceView* const pSRV[1] = { NULL };
		D3D::GetDC()->PSSetShaderResources(0, 1, pSRV);
	}*/
}


