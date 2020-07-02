#include "stdafx.h"
#include "ModelExtractor.h"
#include "Viewer/Freedom.h"

#include "Converter/XmlExtractor.h"


void ModelExtractor::Initialize()
{
	string name = "pikachu";

	/*Exporter* reader = new Exporter("Models/" + name + ".fbx");
	reader->ExporterMaterial(name + "/" + name);*/
	//reader->ExporterMesh(name + "/" + name);
	/*XmlExtractor* extractor = new XmlExtractor();
	auto& loader = Utility::AssetLoader::GetLoader();
	loader.LoadFbx("unitychan", extractor);*/

	auto extractor = XmlExtractor::Create();

	extractor->LoadModel("Autum");
	//extractor->LoadAnimation(0,"kachujin/Idle");

	delete extractor;
	//delete reader;
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

void ModelExtractor::CreateFreedomCamera()
{
}
