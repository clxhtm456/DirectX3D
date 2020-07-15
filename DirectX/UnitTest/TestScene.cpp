#include "stdafx.h"
#include "TestScene.h"
#include "Viewer/Freedom.h"

#include "Renders/Render2D.h"

void TestScene::Initialize()
{
	CreateFreedomCamera();

	cube = MeshCube::Create();
	cube->SetScale(20, 10, 20);
	cube->SetPosition(0, 5, 0);
	cube->GetMaterial()->SetDiffuseMap("../../_Textures/Stones.png");
	cube->GetMaterial()->SetSpecular(1, 1, 1, 1);
	//cube->DelMask(TYPEMASK::DEFAULT);
	AddChild(cube);

	auto cubeInst = cube->CreateInstance();
	cubeInst->SetScale(20, 10, 20);
	cubeInst->SetPosition(-50, 5, 0);
	AddChild(cubeInst);

	auto cubeInst2 = cube->CreateInstance();
	cubeInst2->SetPosition(10, 0, 0);
	cubeInst2->SetScale(20, 10, 20);
	AddChild(cubeInst2);

	
	auto grid = MeshGrid::Create(10,10);
	grid->SetScale(12, 1, 12);
	grid->SetPosition(0, 0, 0);
	grid->GetMaterial()->SetDiffuseMap("../../_Textures/Floor.png");
	grid->GetMaterial()->SetNormalMap("../../_Textures/Floor_Normal.png");
	grid->GetMaterial()->SetSpecularMap("../../_Textures/Floor_Specular.png");
	AddChild(grid);

	auto sphere = MeshSphere::Create(0.5f,20,20);
	sphere->GetMaterial()->SetDiffuseMap("../../_Textures/Wall.png");
	sphere->GetMaterial()->SetSpecularMap("../../_Textures/Wall_Specular.png");
	sphere->GetMaterial()->SetNormalMap("../../_Textures/Wall_Normal.png");
	sphere->GetMaterial()->SetEmissive(Color(1.0f, 0.0f, 0.0f, 1.0f));
	AddChild(sphere);

	auto sphereInst = sphere->CreateInstance();
	sphereInst->SetPosition(5, 20, 0);
	sphereInst->SetScale(5, 5, 5);
	AddChild(sphereInst);

	/*auto cylinder = MeshCylinder::Create(0.5f,3.0f,20,20);
	cylinder->SetScale(5, 5, 5);
	cylinder->SetPosition(-30, 6, -15);
	AddChild(cylinder);*/

	/*auto renderImage = Render2D::Create();
	auto depthRenderTarget = GetDirectionLight()->GetRenderTarget()->SRV();
	renderImage->SetSRV(depthRenderTarget);
	renderImage->SetPosition(150, D3D::Height() - 150, 0);
	renderImage->SetScale(300, 300, 1);
	AddChild(renderImage);*/

	/*auto renderImage2 = Render2D::Create();
	auto cameraRender = GetMainCamera()->GetRenderTarget();
	renderImage2->SetSRV(cameraRender->SRV());
	renderImage2->SetPosition(480, D3D::Height() - 150, 0);
	renderImage2->SetScale(300, 300, 1);
	AddChild(renderImage2);*/

	/*auto terrain = Terrain::Create(512,512);
	terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->SetPosition(0, 0, 0);
	terrain->SetScale(1, 1, 1);
	terrain->SetShader(L"TerrainEdit");
	terrain->UseShadow(false);
	AddChild(terrain);*/

	//auto kachujin = ModelAnim::Create("pikachu");
	////kachujin->SetPosition(20, 0, 0);
	////kachujin->AddClip("Arthas/Idle/Attack2H1");
	//kachujin->AddClip("idle");
	////kachujin->PlayClip(0, true);
	////kachujin->SetScale(0.2f, 0.2f, 0.2f);
	//AddChild(kachujin);

	//auto instModel1 = kachujin->CreateInstance();
	//instModel1->SetScale(0.2f, 0.2f, 0.2f);
	//instModel1->SetPosition(40, 0, 0);
	//AddChild(instModel1);
	//AddChild(instModel2);
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
	//option.useGBuffer = true;

	freedomCam = Freedom::Create(option);
	freedomCam->SetPosition(20, 37, -68);
	freedomCam->SetRotationDegree(30, -20, 0);
	freedomCam->Speed(50, 5);
	/*freedomCam->SetPosition(27, 12, 20);
	freedomCam->SetRotationDegree(20, -157, 0);*/
	//AddChild(freedomCam);
	SetMainCamera(freedomCam);

	//freedomCam->RemoveFromParent();

}
