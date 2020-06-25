#include "stdafx.h"
#include "XmlExtractor.h"

void XmlExtractor::WriteMaterial(FbxScene* scene,wstring savePath,bool bOverWrite)
{
	m_scene = scene;
	if (bOverWrite == false)
	{
		if (Path::ExistFile(savePath) == true)
			return;
	}

	int lMaterialCount = scene->GetMaterialCount();

	string folder = String::ToString(Path::GetDirectoryName(savePath));
	string file = String::ToString(Path::GetFileName(savePath));

	Path::CreateFolders(folder);

	Xml::XMLDocument* document = new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Materials");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	for (int lCount = 0; lCount < lMaterialCount; lCount++)
	{
		Xml::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		Xml::XMLElement* element = NULL;
		
		fbxsdk::FbxSurfaceMaterial* lMaterial = scene->GetMaterial(lCount);
		FbxPropertyT<FbxDouble3> lKFbxDouble3;
		FbxColor theColor;

		element = document->NewElement("Name");
		element->SetText((char*)lMaterial->GetName());
		node->LinkEndChild(element);

		FbxProperty lProperty;
		
		//Diffuse Textures
		lProperty = lMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse);
		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(lProperty).c_str());
		node->LinkEndChild(element);

		//Specular Textures
		lProperty = lMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sSpecular);
		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(lProperty).c_str());
		node->LinkEndChild(element);

		//Normal Textures
		lProperty = lMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sNormalMap);
		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(lProperty).c_str());
		node->LinkEndChild(element);

		if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId) || lMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			lKFbxDouble3 = ((FbxSurfaceLambert*)lMaterial)->Ambient;
			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

			element = document->NewElement("Ambient");
			element->SetAttribute("R", theColor.mRed);
			element->SetAttribute("G", theColor.mGreen);
			element->SetAttribute("B", theColor.mBlue);
			element->SetAttribute("A", theColor.mAlpha);
			node->LinkEndChild(element);

			lKFbxDouble3 = ((FbxSurfaceLambert*)lMaterial)->Diffuse;
			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
			element = document->NewElement("Diffuse");
			element->SetAttribute("R", theColor.mRed);
			element->SetAttribute("G", theColor.mGreen);
			element->SetAttribute("B", theColor.mBlue);
			element->SetAttribute("A", theColor.mAlpha);
			node->LinkEndChild(element);;

			lKFbxDouble3 = ((FbxSurfaceLambert*)lMaterial)->Emissive;
			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
			element = document->NewElement("Emissive");
			element->SetAttribute("R", theColor.mRed);
			element->SetAttribute("G", theColor.mGreen);
			element->SetAttribute("B", theColor.mBlue);
			element->SetAttribute("A", theColor.mAlpha);
			node->LinkEndChild(element);
		}
		else
		{
			element = document->NewElement("Ambient");
			element->SetAttribute("R", 1);
			element->SetAttribute("G", 1);
			element->SetAttribute("B", 1);
			element->SetAttribute("A", 1);
			node->LinkEndChild(element);

			element = document->NewElement("Diffuse");
			element->SetAttribute("R", 1);
			element->SetAttribute("G", 1);
			element->SetAttribute("B", 1);
			element->SetAttribute("A", 1);
			node->LinkEndChild(element);;

			element = document->NewElement("Emissive");
			element->SetAttribute("R", 0);
			element->SetAttribute("G", 0);
			element->SetAttribute("B", 0);
			element->SetAttribute("A", 0);
			node->LinkEndChild(element);
		}
		
	}

	document->SaveFile((folder + file).c_str());
	SafeDelete(document);
}

string XmlExtractor::WriteTexture(FbxProperty& pProperty)
{
	int lTextureCount = pProperty.GetSrcObjectCount<FbxTexture>();

	for (int j = 0; j < lTextureCount; ++j)
	{
		//Here we have to check if it's layeredtextures, or just textures:
		FbxLayeredTexture* lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
		if (lLayeredTexture)
		{
			int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
			for (int k = 0; k < lNbTextures; ++k)
			{
				FbxTexture* lTexture = lLayeredTexture->GetSrcObject<FbxTexture>(k);
				if (lTexture)
				{

					FbxLayeredTexture::EBlendMode lBlendMode;
					lLayeredTexture->GetTextureBlendMode(k, lBlendMode);
					FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(lTexture);
					return (char*)lFileTexture->GetFileName();
				}

			}
		}
		else
		{
			//no layered texture simply get on the property
			FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);
			if (lTexture)
			{
				FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(lTexture);
				return (char*)lFileTexture->GetFileName();
			}
		}
	}

	return "";
}