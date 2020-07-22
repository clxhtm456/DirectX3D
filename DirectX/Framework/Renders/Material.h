#pragma once

class Material
{
public:
public:
	Material();
	Material(wstring diffuseMap, wstring specularMap = L"", wstring normalMap = L"");
	~Material();

	wstring GetName() { return name; }
	void SetName(wstring name);
public:
	Vector3 GetDiffuse() { return materialBuffer->data.diffuse; }
	void SetDiffuse(Vector3 color);
	void SetDiffuse(float r, float g, float b, float a = 1.0f);

	Vector3 GetAmbient() { return materialBuffer->data.ambient; }
	void SetAmbient(Vector3 color);
	void SetAmbient(float r, float g, float b, float a = 1.0f);

	Vector3 GetSpecular() { return materialBuffer->data.specular; }
	void SetSpecular(Vector3 color);
	void SetSpecular(float r, float g, float b, float a = 1.0f);

	Vector3 GetEmissive() { return materialBuffer->data.emissive; }
	void SetEmissive(Vector3 color);
	void SetEmissive(float r, float g, float b, float a = 1.0f);

	Vector3 GetTransparent() { return materialBuffer->data.transparent; }
	void SetTransparent(Vector3 color);
	void SetTransparent(float r, float g, float b, float a = 1.0f);

	Vector3 GetReflective() { return materialBuffer->data.reflective; }
	void SetReflective(Vector3 color);
	void SetReflective(float r, float g, float b, float a = 1.0f);

	float GetBumpscaling() { return materialBuffer->data.bumpscaling; }
	void SetBumpscaling(float value) { materialBuffer->data.bumpscaling = value; }

	float GetOpacity() { return materialBuffer->data.opacity; }
	void SetOpacity(float value) { materialBuffer->data.opacity = value; }

	float GetShininess() { return materialBuffer->data.shininess; }
	void SetShininess(float value) { materialBuffer->data.shininess = value; }

	float GetShininessStrength() { return materialBuffer->data.shininessstrength; }
	void SetShininessStrength(float value) { materialBuffer->data.shininessstrength = value; }

	float GetTransparentFactor() { return materialBuffer->data.transparentfactor; }
	void SetTransparentFactor(float value) { materialBuffer->data.transparentfactor = value; }

	float GetRefracti() { return materialBuffer->data.refracti; }
	void SetRefracti(float value) { materialBuffer->data.refracti = value; }

	int GetOpaque() { return materialBuffer->data.opaque; }
	void SetOpaque(int value) { materialBuffer->data.opaque = value; }

	int GetReflector() { return materialBuffer->data.reflector; }
	void SetReflector(int value) { materialBuffer->data.reflector = value; }

	int HasDiffuseMap() { return materialBuffer->data.hasDiffuseMap; }
	int HasSpecularMap() { return materialBuffer->data.hasSpecularMap; }
	int HasAmbientMap() { return materialBuffer->data.hasAmbientMap; }
	int HasEmissiveMap() { return materialBuffer->data.hasEmissiveMap; }
	int HasHeightMap() { return materialBuffer->data.hasHeightMap; }
	int HasNormalMap() { return materialBuffer->data.hasNormalMap; }
	int HasShininessMap() { return materialBuffer->data.hasShininessMap; }
	int HasOpacityMap() { return materialBuffer->data.hasOpacityMap; }
	int HasDisplacementMap() { return materialBuffer->data.hasDisplacementMap; }
	int HasLightMapMap() { return materialBuffer->data.hasLightMapMap; }
	int HasReflectionMap() { return materialBuffer->data.hasReflectionMap; }
	int HasBasecolorMap() { return materialBuffer->data.hasBasecolorMap; }
	int HasNormalcameraMap() { return materialBuffer->data.hasNormalcameraMap; }
	int HasEmissioncolorMap() { return materialBuffer->data.hasEmissioncolorMap; }
	int HasMetalnessMap() { return materialBuffer->data.hasMetalnessMap; }
	int HasDiffuseroughnessMap() { return materialBuffer->data.hasDiffuseroughnessMap; }
	int HasAmbientocculsionMap() { return materialBuffer->data.hasAmbientocculsionMap; }

	ID3D11ShaderResourceView* diffusesrv = nullptr;
	ID3D11ShaderResourceView* specularsrv = nullptr;
	ID3D11ShaderResourceView* normalsrv = nullptr;
	ID3D11ShaderResourceView* ambientsrv = nullptr;
	ID3D11ShaderResourceView* emissivesrv = nullptr;
	ID3D11ShaderResourceView* heightsrv = nullptr;
	ID3D11ShaderResourceView* shininesssrv = nullptr;
	ID3D11ShaderResourceView* opacitysrv = nullptr;
	ID3D11ShaderResourceView* displacementsrv = nullptr;
	ID3D11ShaderResourceView* lightMapsrv = nullptr;
	ID3D11ShaderResourceView* reflectionsrv = nullptr;
	//PBR Stingray                   
	ID3D11ShaderResourceView* basecolorsrv = nullptr;
	ID3D11ShaderResourceView* normalcamerasrv = nullptr;
	ID3D11ShaderResourceView* emissioncolorsrv = nullptr;
	ID3D11ShaderResourceView* metalnesssrv = nullptr;
	ID3D11ShaderResourceView* diffuseroughnesssrv = nullptr;
	ID3D11ShaderResourceView* ambientocculsionsrv = nullptr;

	ID3D11ShaderResourceView* GetDiffuseMap() { return diffusesrv; }
	void SetDiffuseMap(string file);
	void SetDiffuseMap(wstring file);

	ID3D11ShaderResourceView* GetSpecularMap() { return specularsrv; }
	void SetSpecularMap(string file);
	void SetSpecularMap(wstring file);

	ID3D11ShaderResourceView* GetNormalMap() { return normalsrv; }
	void SetNormalMap(string file);
	void SetNormalMap(wstring file);

	ID3D11ShaderResourceView* GetAmbientMap() { return ambientsrv; }
	void SetAmbientMap(string file);
	void SetAmbientMap(wstring file);

	ID3D11ShaderResourceView* GetEmissiveMap() { return emissivesrv; }
	void SetEmissiveMap(string file);
	void SetEmissiveMap(wstring file);

	ID3D11ShaderResourceView* GetHeightMap() { return heightsrv; }
	void SetHeightMap(string file);
	void SetHeightMap(wstring file);

	ID3D11ShaderResourceView* GetShinnessMap() { return shininesssrv; }
	void SetShinnessMap(string file);
	void SetShinnessMap(wstring file);

	ID3D11ShaderResourceView* GetOpacityMap() { return opacitysrv; }
	void SetOpacityMap(string file);
	void SetOpacityMap(wstring file);

	ID3D11ShaderResourceView* GetDisplacementMap() { return displacementsrv; }
	void SetDisplacementMap(string file);
	void SetDisplacementMap(wstring file);

	ID3D11ShaderResourceView* GetLightMap() { return lightMapsrv; }
	void SetLightMap(string file);
	void SetLightMap(wstring file);

	ID3D11ShaderResourceView* GetReflectionMap() { return reflectionsrv; }
	void SetReflectionMap(string file);
	void SetReflectionMap(wstring file);

	ID3D11ShaderResourceView* GetBaseColorMap() { return basecolorsrv; }
	void SetBaseColorMap(string file);
	void SetBaseColorMap(wstring file);

	ID3D11ShaderResourceView* GetNormalCameraMap() { return normalcamerasrv; }
	void SetNormalCameraMap(string file);
	void SetNormalCameraMap(wstring file);

	ID3D11ShaderResourceView* GetEmissionColorMap() { return emissioncolorsrv; }
	void SetEmissionColorMap(string file);
	void SetEmissionColorMap(wstring file);

	ID3D11ShaderResourceView* GetMetalnessMap() { return metalnesssrv; }
	void SetMetalnessMap(string file);
	void SetMetalnessMap(wstring file);

	ID3D11ShaderResourceView* GetDiffuseroughnessMap() { return diffuseroughnesssrv; }
	void SetDiffuseroughnessMap(string file);
	void SetDiffuseroughnessMap(wstring file);

	ID3D11ShaderResourceView* GetAmbientocculsionMap() { return ambientocculsionsrv; }
	void SetAmbientocculsionMap(string file);
	void SetAmbientocculsionMap(wstring file);
public:

	MaterialBuffer* GetBuffer() { return materialBuffer; }


	void Binding();

private:
	void Initialize();

private:
	MaterialBuffer* materialBuffer;
private:
	wstring name;

	ID3D11ShaderResourceView* diffusesrv = nullptr;
	ID3D11ShaderResourceView* specularsrv = nullptr;
	ID3D11ShaderResourceView* normalsrv = nullptr;
	ID3D11ShaderResourceView* ambientsrv = nullptr;
	ID3D11ShaderResourceView* emissivesrv = nullptr;
	ID3D11ShaderResourceView* heightsrv = nullptr;
	ID3D11ShaderResourceView* shininesssrv = nullptr;
	ID3D11ShaderResourceView* opacitysrv = nullptr;
	ID3D11ShaderResourceView* displacementsrv = nullptr;
	ID3D11ShaderResourceView* lightMapsrv = nullptr;
	ID3D11ShaderResourceView* reflectionsrv = nullptr;
	//PBR Stingray                   
	ID3D11ShaderResourceView* basecolorsrv = nullptr;
	ID3D11ShaderResourceView* normalcamerasrv = nullptr;
	ID3D11ShaderResourceView* emissioncolorsrv = nullptr;
	ID3D11ShaderResourceView* metalnesssrv = nullptr;
	ID3D11ShaderResourceView* diffuseroughnesssrv = nullptr;
	ID3D11ShaderResourceView* ambientocculsionsrv = nullptr;
};