#include "stdafx.h"
#include "ModelExtractor.h"

#include"Converter/XmlExtractor.h"

void ModelExtractor::Initialize()
{
	/*Exporter* reader = new Exporter("Models/" + name + ".fbx");
	reader->ExporterMaterial(name + "/" + name);*/
	//reader->ExporterMesh(name + "/" + name);
	/*XmlExtractor* extractor = new XmlExtractor();
	auto& loader = Utility::AssetLoader::GetLoader();
	loader.LoadFbx("unitychan", extractor);*/

	auto extractor = XmlExtractor::Create();
	extractor->LoadAnimation(0,"Pikachu/idle");
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
