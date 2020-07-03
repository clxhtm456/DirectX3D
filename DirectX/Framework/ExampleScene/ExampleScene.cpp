#include "Framework.h"
#include "ExampleScene.h"

#include "Viewer/Freedom.h"
#include "Renders/Render2D.h"

//�� �ʱ�ȭ
void ExampleScene::Initialize()
{
	//Test�� �����̴� ī�޶� �߰�
	CreateFreedomCamera();

	//�׽�Ʈ�� Mesh �߰�
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
	sphere->GetMaterial()->SetEmissive(Color(1.0f, 0.0f, 0.0f, 1.0f));
	AddChild(sphere);

	auto cylinder = MeshCylinder::Create(0.5f,3.0f,20,20);
	cylinder->SetScale(5, 5, 5);
	cylinder->SetPosition(-30, 6, -15);
	AddChild(cylinder);

}

//�� ����� ȣ��
/*
Addchild�� ���� ���� �߰��� ������Ʈ���� ������ �޸� ������
*/
void ExampleScene::Destroy()
{
}

//�������� ȣ��
/*
Addchild�� ���� �ڽ����� ���ϰԵ� Node�� �θ� Update�� ȣ��� �ڵ����� Update�� ȣ����
*/
void ExampleScene::Update()
{
	if (Keyboard::Get()->Press(VK_UP))
	{
		//cube->SetShader(L"RedMesh");
	}
}

//���������� ȣ��
void ExampleScene::PreRender()
{
}

//�� ������ ������ ȣ��
/*
�� ī�޶� �� �������� �� ���� Node ������
���� ī�޶� ������
*/
void ExampleScene::Render()
{
}

//������ ���� ȣ��
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
