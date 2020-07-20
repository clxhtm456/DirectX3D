#include "stdafx.h"
#include "ModelExtractor.h"

#include"Converter/XmlExtractor.h"
#include "AssimpModelLoader/AssimpConverter.h"

void ModelExtractor::Initialize()
{
	/*Exporter* reader = new Exporter("Models/" + name + ".fbx");
	reader->ExporterMaterial(name + "/" + name);*/
	//reader->ExporterMesh(name + "/" + name);
	/*XmlExtractor* extractor = new XmlExtractor();
	auto& loader = Utility::AssetLoader::GetLoader();
	loader.LoadFbx("unitychan", extractor);*/

	auto extractor = new AssimpConverter();
	extractor->ConvertMesh("../../_Assets/Models/Kachujin/Kachujin.fbx");
	delete extractor;

}

void ModelExtractor::Destroy()
{
}

void ModelExtractor::Update()
{
}

void ModelExtractor::PreRender()
{
}

void ModelExtractor::Render()
{
}

void ModelExtractor::PostRender()
{
}
