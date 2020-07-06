#include "stdafx.h"
#include "TerrainEditScene.h"
#include "Viewer/Freedom.h"

#include "MapEditor/MapEditor.h"

void TerrainEditScene::Initialize()
{
	CreateFreedomCamera();

	mapEditor = new MapEditor(this);

	//auto a = sin(50);

}

void TerrainEditScene::Destroy()
{
	delete mapEditor;
}

void TerrainEditScene::Update()
{
	mapEditor->Update();
}

void TerrainEditScene::PreRender()
{
}

void TerrainEditScene::Render()
{
	mapEditor->Render();
}

void TerrainEditScene::PostRender()
{
}

void TerrainEditScene::CreateFreedomCamera()
{
	D3DDesc desc = D3D::GetDesc();
	CameraOption option;
	option.zf = 2000.0f;
	option.Width = desc.Width;
	option.Height = desc.Height;
	//option.useGBuffer = true;

	freedomCam = Freedom::Create(option);
	freedomCam->SetPosition(0, 0, 0);
	freedomCam->SetRotationDegree(40, 0, 0);
	freedomCam->Speed(100, 5);
	SetMainCamera(freedomCam);

	//freedomCam->RemoveFromParent();

}
