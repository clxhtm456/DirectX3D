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

	Color GetAmbient() { return materialBuffer->data.ambient; }
	void SetAmbient(Color color);
	void SetAmbient(float r, float g, float b, float a = 1.0f);

	Color GetDiffuse() { return materialBuffer->data.diffuse; }
	void SetDiffuse(Color color);
	void SetDiffuse(float r, float g, float b, float a = 1.0f);

	Color GetSpecular() { return materialBuffer->data.specular; }
	void SetSpecular(Color color);
	void SetSpecular(float r, float g, float b, float a = 1.0f);

	Color GetEmissive() { return materialBuffer->data.emissive; }
	void SetEmissive(Color color);
	void SetEmissive(float r, float g, float b, float a = 1.0f);

	MaterialBuffer* GetBuffer() { return materialBuffer; }

	Texture* GetDiffuseMap() { return diffuseMap; }
	void SetDiffuseMap(string file);
	void SetDiffuseMap(wstring file);

	Texture* GetSpecularMap() { return specularMap; }
	void SetSpecularMap(string file);
	void SetSpecularMap(wstring file);

	Texture* GetNormalMap() { return normalMap; }
	void SetNormalMap(string file);
	void SetNormalMap(wstring file);

	void Render();

private:
	void Initialize();

private:
	MaterialBuffer* materialBuffer;
private:
	wstring name;

	Texture* diffuseMap;
	Texture* specularMap;
	Texture* normalMap;
};