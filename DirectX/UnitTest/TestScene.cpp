#include "stdafx.h"
#include "TestScene.h"
#include "Viewer/Freedom.h"

#include "Renders/Render2D.h"

void TestScene::Initialize()
{
	CreateFreedomCamera();

	cube = MeshCube::Create();
	cube->GetMaterial()->SetDiffuseMap("../../_Textures/Stones.png");
	cube->GetMaterial()->SetSpecularMap("../../_Textures/Stones_Specular.png");
	cube->GetMaterial()->SetNormalMap("../../_Textures/Stones_Normal.png");
	cube->GetMaterial()->SetSpecular(1, 1, 1, 20);
	cube->GetMaterial()->SetEmissive(0.3f, 0.3f, 0.3f, 0.3f);
	cube->SetScale(20, 10, 20);
	cube->SetPosition(0, 5, 0);
	AddChild(cube);

	auto grid = MeshGrid::Create(5, 5);
	grid->GetMaterial()->SetSpecular(1, 1, 1, 20);
	grid->GetMaterial()->SetEmissive(0.3f, 0.3f, 0.3f, 0.3f);
	grid->GetMaterial()->SetDiffuseMap("../../_Textures/Floor.png");
	grid->GetMaterial()->SetNormalMap("../../_Textures/Floor_Normal.png");
	grid->GetMaterial()->SetSpecularMap("../../_Textures/Floor_Specular.png");
	grid->SetPosition(0, 0, 0);
	grid->SetScale(12, 1, 12);
	AddChild(grid);

	auto cylinder = MeshCylinder::Create(0.5f, 3.0f, 20, 20);
	cylinder->GetMaterial()->SetDiffuseMap("../../_Textures/Bricks.png");
	cylinder->GetMaterial()->SetNormalMap("../../_Textures/Bricks_Normal.png");
	cylinder->GetMaterial()->SetSpecularMap("../../_Textures/Bricks_Specular.png");
	cylinder->GetMaterial()->SetSpecular(1, 1, 1, 20);
	cylinder->GetMaterial()->SetEmissive(0.3f, 0.3f, 0.3f, 0.3f);
	AddChild(cylinder);

	auto sphere = MeshSphere::Create(0.5f, 20, 20);
	sphere->GetMaterial()->SetDiffuseMap("../../_Textures/Wall.png");
	sphere->GetMaterial()->SetSpecularMap("../../_Textures/Wall_Specular.png");
	sphere->GetMaterial()->SetNormalMap("../../_Textures/Wall_Normal.png");
	sphere->GetMaterial()->SetSpecular(1, 1, 1, 20);
	sphere->GetMaterial()->SetEmissive(0.3f, 1.0f, 0.3f, 0.5f);
	AddChild(sphere);
	
	for (UINT i = 0; i < 5; i++)
	{
		auto sphereInst1 = sphere->CreateInstance();
		sphereInst1->SetPosition(-30, 15.5f, -15.0f + (float)i * 15.0f);
		sphereInst1->SetScale(5, 5, 5);
		AddChild(sphereInst1);

		auto sphereInst2 = sphere->CreateInstance();
		sphereInst2->SetPosition(30, 15.5f, -15.0f + (float)i * 15.0f);
		sphereInst2->SetScale(5, 5, 5);
		AddChild(sphereInst2);

		auto cylinderInst1 = cylinder->CreateInstance();
		cylinderInst1->SetPosition(30, 6, -15.0f + (float)i * 15.0f);
		cylinderInst1->SetScale(5, 5, 5);
		AddChild(cylinderInst1);

		auto cylinderInst2 = cylinder->CreateInstance();
		cylinderInst2->SetPosition(-30, 6, -15.0f + (float)i * 15.0f);
		cylinderInst2->SetScale(5, 5, 5);
		AddChild(cylinderInst2);

	}

	/*auto kachujin = ModelAnim::Create("pikachu");
	kachujin->AddClip("idle");
	AddChild(kachujin);

	auto instModel1 = kachujin->CreateInstance();
	instModel1->SetScale(0.075f, 0.075f, 0.075f);
	instModel1->SetPosition(-25, 0, -30);
	AddChild(instModel1);

	auto instModel2 = kachujin->CreateInstance();
	instModel2->SetScale(0.075f, 0.075f, 0.075f);
	instModel2->SetPosition(-10, 0, -30);
	AddChild(instModel2);

	auto instModel3 = kachujin->CreateInstance();
	instModel3->SetScale(0.075f, 0.075f, 0.075f);
	instModel3->SetPosition(10, 0, -30);
	AddChild(instModel3);

	auto instModel4 = kachujin->CreateInstance();
	instModel4->SetScale(0.075f, 0.075f, 0.075f);
	instModel4->SetPosition(25, 0, -30);
	AddChild(instModel4);*/
}

void TestScene::Destroy()
{
}

void TestScene::Update()
{
	if (Keyboard::Get()->Press(VK_UP))
	{
		cube->SetPSShader(L"RedMesh");
	}
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
	option.useGBuffer = true;

	freedomCam = Freedom::Create(option);
	freedomCam->SetPosition(0, 32, -67);
	freedomCam->SetRotationDegree(23, 0, 0);
	freedomCam->Speed(50, 5);
	/*freedomCam->SetPosition(27, 12, 20);
	freedomCam->SetRotationDegree(20, -157, 0);*/
	//AddChild(freedomCam);
	SetMainCamera(freedomCam);

	//freedomCam->RemoveFromParent();

}
