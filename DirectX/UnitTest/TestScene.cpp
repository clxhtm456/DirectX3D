#include "stdafx.h"
#include "TestScene.h"
#include "Viewer/Freedom.h"

void TestScene::Initialize()
{
	CreateFreedomCamera();

	//auto mesh = MeshCube::Create();
	auto mesh = MeshSphere::Create(5);
	mesh->SetScale(1, 1, 1);
	mesh->SetPosition(0, 0, 0);

	auto terrain = Terrain::Create(L"Terrain/Gray1024.jpg");
	terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->SetPosition(0, 0, 0);
	//terrain->SetScale(2, 2, 2);

	AddChild(mesh);
	AddChild(terrain);
}

void TestScene::Destroy()
{
}

void TestScene::Update()
{
}

void TestScene::PreRender()
{
}

void TestScene::Render()
{
}

void TestScene::PostRender()
{
}

void TestScene::CreateFreedomCamera()
{
	D3DDesc desc = D3D::GetDesc();
	CameraOption option;
	option.Width = desc.Width;
	option.Height = desc.Height;

	freedomCam = Freedom::Create(option);
	freedomCam->SetPosition(0, 0, -30);

	SetMainCamera(freedomCam);

}
