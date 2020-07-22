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
}

Material::~Material()
{
	delete materialBuffer;
}

void Material::SetName(wstring name)
{
	this->name = name;
}

void Material::SetAmbient(Vector3 color)
{
	materialBuffer->data.ambient = color;
}

void Material::SetAmbient(float r, float g, float b, float a)
{
	Vector3 temp = Vector3(r, g, b);
	SetAmbient(temp);
}


void Material::SetDiffuse(Vector3 color)
{
	materialBuffer->data.diffuse = color;
}

void Material::SetDiffuse(float r, float g, float b, float a)
{
	Vector3 temp = Vector3(r, g, b);
	SetDiffuse(temp);
}

void Material::SetSpecular(Vector3 color)
{
	materialBuffer->data.specular = color;
}

void Material::SetSpecular(float r, float g, float b, float a)
{
	Vector3 temp = Vector3(r, g, b);
	SetSpecular(temp);
}

void Material::SetEmissive(Vector3 color)
{
	materialBuffer->data.emissive = color;
}

void Material::SetEmissive(float r, float g, float b, float a)
{
	Vector3 temp = Vector3(r, g, b);
	SetEmissive(temp);
}

void Material::SetTransparent(Vector3 color)
{
	materialBuffer->data.transparent = color;
}

void Material::SetTransparent(float r, float g, float b, float a)
{
	Vector3 temp = Vector3(r, g, b);
	SetTransparent(temp);
}

void Material::SetReflective(Vector3 color)
{
	materialBuffer->data.reflective = color;
}

void Material::SetReflective(float r, float g, float b, float a)
{
	Vector3 temp = Vector3(r, g, b);
	SetReflective(temp);
}


void Material::SetDiffuseMap(string file)
{
	SetDiffuseMap(String::ToWString(file));
}

void Material::SetDiffuseMap(wstring file)
{
	diffusesrv = Texture::LoadSRV(file);

	int result = 0;
	if (diffusesrv != nullptr)
		result = 1;

	materialBuffer->data.hasDiffuseMap = result;
}

void Material::SetSpecularMap(string file)
{
	SetSpecularMap(String::ToWString(file));
}

void Material::SetSpecularMap(wstring file)
{
	specularsrv = Texture::LoadSRV(file);

	int result = 0;
	if (specularsrv != nullptr)
		result = 1;
	materialBuffer->data.hasSpecularMap = result;
}

void Material::SetNormalMap(string file)
{
	SetNormalMap(String::ToWString(file));
}

void Material::SetNormalMap(wstring file)
{
	normalsrv = Texture::LoadSRV(file);

	int result = 0;
	if (normalsrv != nullptr)
		result = 1;
	materialBuffer->data.hasNormalcameraMap = result;
}

void Material::SetAmbientMap(string file)
{
	SetAmbientMap(String::ToWString(file));
}

void Material::SetAmbientMap(wstring file)
{
	ambientsrv = Texture::LoadSRV(file);

	int result = 0;
	if (ambientsrv != nullptr)
		result = 1;
	materialBuffer->data.hasAmbientMap = result;
}

void Material::SetEmissiveMap(string file)
{
	SetEmissiveMap(String::ToWString(file));
}

void Material::SetEmissiveMap(wstring file)
{
	emissivesrv = Texture::LoadSRV(file);

	int result = 0;
	if (emissivesrv != nullptr)
		result = 1;
	materialBuffer->data.hasEmissiveMap = result;
}

void Material::SetHeightMap(string file)
{
	SetHeightMap(String::ToWString(file));
}

void Material::SetHeightMap(wstring file)
{
	heightsrv = Texture::LoadSRV(file);

	int result = 0;
	if (heightsrv != nullptr)
		result = 1;
	materialBuffer->data.hasHeightMap = result;
}

void Material::SetShinnessMap(string file)
{
	SetShinnessMap(String::ToWString(file));
}

void Material::SetShinnessMap(wstring file)
{
	shininesssrv = Texture::LoadSRV(file);

	int result = 0;
	if (shininesssrv != nullptr)
		result = 1;
	materialBuffer->data.hasShininessMap = result;
}

void Material::SetOpacityMap(string file)
{
	SetOpacityMap(String::ToWString(file));
}

void Material::SetOpacityMap(wstring file)
{
	opacitysrv = Texture::LoadSRV(file);

	int result = 0;
	if (opacitysrv != nullptr)
	{
		materialBuffer->data.opaque = 0;
		result = 1;
	}
	materialBuffer->data.hasOpacityMap = result;
}

void Material::SetDisplacementMap(string file)
{
	SetDisplacementMap(String::ToWString(file));
}

void Material::SetDisplacementMap(wstring file)
{
	displacementsrv = Texture::LoadSRV(file);

	int result = 0;
	if (displacementsrv != nullptr)
		result = 1;
	materialBuffer->data.hasDisplacementMap = result;
}

void Material::SetLightMap(string file)
{
	SetLightMap(String::ToWString(file));
}

void Material::SetLightMap(wstring file)
{
	lightMapsrv = Texture::LoadSRV(file);

	int result = 0;
	if (lightMapsrv != nullptr)
		result = 1;
	materialBuffer->data.hasLightMapMap = result;
}

void Material::SetReflectionMap(string file)
{
	SetReflectionMap(String::ToWString(file));
}

void Material::SetReflectionMap(wstring file)
{
	reflectionsrv = Texture::LoadSRV(file);

	int result = 0;
	if (reflectionsrv != nullptr)
		result = 1;
	materialBuffer->data.hasReflectionMap = result;
}

void Material::SetBaseColorMap(string file)
{
	SetBaseColorMap(String::ToWString(file));
}

void Material::SetBaseColorMap(wstring file)
{
	basecolorsrv = Texture::LoadSRV(file);

	int result = 0;
	if (basecolorsrv != nullptr)
		result = 1;
	materialBuffer->data.hasBasecolorMap = result;
}

void Material::SetNormalCameraMap(string file)
{
	SetNormalCameraMap(String::ToWString(file));
}

void Material::SetNormalCameraMap(wstring file)
{
	normalcamerasrv = Texture::LoadSRV(file);

	int result = 0;
	if (normalcamerasrv != nullptr)
		result = 1;
	materialBuffer->data.hasNormalcameraMap = result;
}

void Material::SetEmissionColorMap(string file)
{
	SetEmissionColorMap(String::ToWString(file));
}

void Material::SetEmissionColorMap(wstring file)
{
	emissioncolorsrv = Texture::LoadSRV(file);

	int result = 0;
	if (emissioncolorsrv != nullptr)
		result = 1;
	materialBuffer->data.hasEmissioncolorMap = result;
}

void Material::SetMetalnessMap(string file)
{
	SetMetalnessMap(String::ToWString(file));
}

void Material::SetMetalnessMap(wstring file)
{
	metalnesssrv = Texture::LoadSRV(file);

	int result = 0;
	if (metalnesssrv != nullptr)
		result = 1;
	materialBuffer->data.hasMetalnessMap = result;
}

void Material::SetDiffuseroughnessMap(string file)
{
	SetDiffuseroughnessMap(String::ToWString(file));
}

void Material::SetDiffuseroughnessMap(wstring file)
{
	diffuseroughnesssrv = Texture::LoadSRV(file);

	int result = 0;
	if (diffuseroughnesssrv != nullptr)
		result = 1;
	materialBuffer->data.hasDiffuseroughnessMap = result;
}

void Material::SetAmbientocculsionMap(string file)
{
	SetAmbientocculsionMap(String::ToWString(file));
}

void Material::SetAmbientocculsionMap(wstring file)
{
	ambientocculsionsrv = Texture::LoadSRV(file);

	int result = 0;
	if (ambientocculsionsrv != nullptr)
		result = 1;
	materialBuffer->data.hasAmbientocculsionMap = result;
}

void Material::Binding()
{
	materialBuffer->SetPSBuffer(PS_MATERIALBUFFER);

	D3D::GetDC()->PSSetShaderResources(0, 1, &diffusesrv);
	D3D::GetDC()->PSSetShaderResources(1, 1, &specularsrv);
	D3D::GetDC()->PSSetShaderResources(2, 1, &normalsrv);
	D3D::GetDC()->PSSetShaderResources(3, 1, &emissivesrv);
	D3D::GetDC()->PSSetShaderResources(4, 1, &heightsrv);
	D3D::GetDC()->PSSetShaderResources(5, 1, &ambientsrv);
	D3D::GetDC()->PSSetShaderResources(6, 1, &shininesssrv);
	D3D::GetDC()->PSSetShaderResources(7, 1, &opacitysrv);
	D3D::GetDC()->PSSetShaderResources(8, 1, &displacementsrv);
	D3D::GetDC()->PSSetShaderResources(9, 1, &lightMapsrv);
	D3D::GetDC()->PSSetShaderResources(10, 1,&reflectionsrv);
	//pbr
	D3D::GetDC()->PSSetShaderResources(11, 1, &basecolorsrv);
	D3D::GetDC()->PSSetShaderResources(12, 1, &normalcamerasrv);
	D3D::GetDC()->PSSetShaderResources(13, 1, &emissioncolorsrv);
	D3D::GetDC()->PSSetShaderResources(14, 1, &metalnesssrv);
	D3D::GetDC()->PSSetShaderResources(15, 1, &diffuseroughnesssrv);
	D3D::GetDC()->PSSetShaderResources(16, 1, &ambientocculsionsrv);

	if (opacitysrv != nullptr || materialBuffer->data.transparent.x > 0.f || materialBuffer->data.transparent.y > 0.f || materialBuffer->data.transparent.z > 0.f)
		materialBuffer->data.opaque = 0;
	if (reflectionsrv != NULL || materialBuffer->data.reflective.x > 0.f || materialBuffer->data.reflective.y > 0.f || materialBuffer->data.reflective.z > 0.f)
		materialBuffer->data.reflector = 1;

	if (ambientsrv != NULL)
		materialBuffer->data.hasAmbientMap = 1;
	if (emissivesrv != NULL)
		materialBuffer->data.hasEmissiveMap = 1;
	if (opacitysrv != NULL)
		materialBuffer->data.hasOpacityMap = 1;
}


