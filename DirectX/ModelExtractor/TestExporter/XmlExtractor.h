#pragma once

class XmlExtractor
{
public:
	void WriteMaterial(FbxScene* scene,wstring savePath,bool bOverWrite);

	string WriteTexture(FbxProperty& pProperty);

	const FbxScene* m_scene;
private:
};