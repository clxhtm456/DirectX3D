#include "stdafx.h"
#include "ModelExtractor.h"
#include "Viewer/Freedom.h"

#include "Exporter/Exporter.h"

void ModelExtractor::Initialize()
{
	string name = "unitychan";

	Exporter* reader = new Exporter("Models/" + name + ".fbx");
	reader->ExporterMaterial(name + "/" + name);
	//reader->ExporterMesh(name + "/" + name);
	delete reader;
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
