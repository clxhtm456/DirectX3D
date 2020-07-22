#include "Framework.h"
#include "ExampleScene.h"

#include "Viewer/Freedom.h"
#include "Renders/Render2D.h"

//씬 초기화
void ExampleScene::Initialize()
{
	//Test용 움직이는 카메라 추가
	CreateFreedomCamera();

	//테스트용 Mesh 추가
	auto cube = MeshCube::Create();
	cube->SetScale(20, 10, 20);
	cube->SetPosition(0, 5, 0);
	cube->GetMaterial()->SetDiffuseMap("../../_Textures/Stones.png");
	cube->GetMaterial()->SetSpecular(1, 1, 1, 1);
	AddChild(cube);
	
	auto grid = MeshGrid::Create(10,10);
	grid->SetScale(12, 1, 12);
	grid->SetPosition(0, 0, 0);
	grid->GetMaterial()->SetDiffuseMap("../../_Textures/Floor.png");
	grid->GetMaterial()->SetNormalMap("../../_Textures/Floor_Normal.png");
	grid->GetMaterial()->SetSpecularMap("../../_Textures/Floor_Specular.png");
	AddChild(grid);

	auto sphere = MeshSphere::Create(0.5f,20,20);
	sphere->SetScale(5, 5, 5);
	sphere->SetPosition(-30,15,-15);
	sphere->GetMaterial()->SetDiffuseMap("../../_Textures/Wall.png");
	sphere->GetMaterial()->SetSpecularMap("../../_Textures/Wall_Specular.png");
	sphere->GetMaterial()->SetNormalMap("../../_Textures/Wall_Normal.png");
	sphere->GetMaterial()->SetEmissive(1.0f, 0.0f, 0.0f);
	AddChild(sphere);

	auto cylinder = MeshCylinder::Create(0.5f,3.0f,20,20);
	cylinder->SetScale(5, 5, 5);
	cylinder->SetPosition(-30, 6, -15);
	AddChild(cylinder);

}

//씬 종료시 호출
/*
Addchild를 통해 씬에 추가된 오브젝트들은 스스로 메모리 해제됨
*/
void ExampleScene::Destroy()
{
}

//매프레임 호출
/*
Addchild를 통해 자식으로 속하게된 Node는 부모가 Update를 호출시 자동으로 Update를 호출함
*/
void ExampleScene::Update()
{
	if (Keyboard::Get()->Press(VK_UP))
	{
		//cube->SetShader(L"RedMesh");
	}
}

//랜더링전에 호출
void ExampleScene::PreRender()
{
}

//매 프레임 렌더링 호출
/*
씬 카메라 의 개수별로 각 씬의 Node 랜더링
이후 카메라 랜더링
*/
void ExampleScene::Render()
{
}

//랜더링 이후 호출
void ExampleScene::PostRender()
{
}

void ExampleScene::CreateFreedomCamera()
{
	D3DDesc desc = D3D::GetDesc();
	CameraOption option;
	option.Width = desc.Width;
	option.Height = desc.Height;

	freedomCam = Freedom::Create(option);
	freedomCam->SetPosition(20, 37, -68);
	freedomCam->SetRotationDegree(30, -20, 0);

	SetMainCamera(freedomCam);

}
