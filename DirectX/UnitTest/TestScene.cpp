#include "stdafx.h"
#include "TestScene.h"
#include "Viewer/Freedom.h"

void TestScene::Initialize()
{
	shader = Shader::Add(L"Texture");

	CreateFreedomCamera();

	cube = MeshCube::Create();
	cube->SetScale(0.5, 1, 0.5);

	AddChild(cube);
}

void TestScene::Destroy()
{
	delete freedomCam;
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

	freedomCam = new Freedom(option);

	Context::Get()->SetMainCamera(freedomCam);
}
