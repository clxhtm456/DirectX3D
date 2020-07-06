#include "stdafx.h"
#include "MapEditScene.h"
#include "Viewer/Freedom.h"

#include "MapEditor/MapEditor.h"

void MapEditScene::Initialize()
{
	CreateFreedomCamera();

	mapEditor = new MapEditor();

}

void MapEditScene::Destroy()
{
	delete mapEditor;
}

void MapEditScene::Update()
{
	mapEditor->Update();

}

void MapEditScene::PreRender()
{
}

void MapEditScene::Render()
{
	mapEditor->Render();
}

void MapEditScene::PostRender()
{
}

void MapEditScene::CreateFreedomCamera()
{
	D3DDesc desc = D3D::GetDesc();
	CameraOption option;
	option.Width = desc.Width;
	option.Height = desc.Height;
	//option.useGBuffer = true;

	freedomCam = Freedom::Create(option);
	freedomCam->SetPosition(27, 12, 20);
	freedomCam->SetRotationDegree(20, -157, 0);
	//AddChild(freedomCam);
	SetMainCamera(freedomCam);

	//freedomCam->RemoveFromParent();

}
