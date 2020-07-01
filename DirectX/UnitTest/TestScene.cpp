#include "stdafx.h"
#include "TestScene.h"
#include "Viewer/Freedom.h"

void TestScene::Initialize()
{
	CreateFreedomCamera();

	auto mesh = MeshSphere::Create(2);
	mesh->SetScale(1, 1, 1);
	mesh->SetPosition(0, 0, 0);

	/*auto terrain = Terrain::Create(L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->SetPosition(0, 0, 0);
	terrain->SetScale(2, 2, 2);*/

	AddChild(mesh);
	//AddChild(terrain);

	//auto kachujin = Model::Create("pikachu");
	//kachujin->SetScale(0.05f, 0.05f, 0.05f);

	//AddChild(kachujin);
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
	freedomCam->SetPosition(16, 13, -22);
	freedomCam->SetRotationDegree(28, -35, 0);

	SetMainCamera(freedomCam);

}
