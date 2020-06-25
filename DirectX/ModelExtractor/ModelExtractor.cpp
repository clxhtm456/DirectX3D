#include "stdafx.h"
#include "ModelExtractor.h"
#include "Viewer/Freedom.h"

#include "Exporter/Exporter.h"

#include "TestExporter/AssetLoader.h"
#include "TestExporter/FbxLoader.h"

void ModelExtractor::Initialize()
{
	string name = "pikachu";

	/*Exporter* reader = new Exporter("Models/" + name + ".fbx");
	reader->ExporterMaterial(name + "/" + name);*/
	//reader->ExporterMesh(name + "/" + name);

	auto& loader = Utility::AssetLoader::GetLoader();
	loader.LoadFbx("../../_Assets/Models/pikachu.fbx");

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
