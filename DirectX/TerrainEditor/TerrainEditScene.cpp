#include "stdafx.h"
#include "TerrainEditScene.h"
#include "Viewer/Freedom.h"

#include "MapEditor/MapEditor.h"

void TerrainEditScene::Initialize()
{
	CreateFreedomCamera();

	_directionLight->UseShadow(false);

	mapEditor = new MapEditor(this);

	cube = MeshCube::Create();
	//cube->SetScale(20, 10, 20);
	//cube->SetPosition(0, 5, 0);
	cube->GetMaterial()->SetDiffuseMap("../../_Textures/Stones.png");
	cube->GetMaterial()->SetSpecular(1, 1, 1, 1);
	//cube->DelMask(TYPEMASK::DEFAULT);
	AddChild(cube);

	auto cubeInst = cube->CreateInstance();
	cubeInst->SetScale(20, 10, 20);
	cubeInst->SetPosition(0, 5, 0);
	AddChild(cubeInst);

	auto terrain = Terrain::Create(512, 512);
	terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->SetPosition(0, 0, 0);
	terrain->SetScale(1, 1, 1);
	terrain->UseShadow(false);
	//AddChild(terrain);

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
	freedomCam->SetPosition(20, 37, -68);
	freedomCam->SetRotationDegree(30, -20, 0);
	freedomCam->Speed(100, 5);
	SetMainCamera(freedomCam);

	//freedomCam->RemoveFromParent();

}
