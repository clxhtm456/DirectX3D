#include "stdafx.h"
#include "TestScene.h"
#include "Viewer/Freedom.h"

void TestScene::Initialize()
{
	shader = Shader::Add(L"Vertex");
	Vertex vertices[] =
	{
		Vector3(0.0f, 0.5f, 0.0f),
		Vector3(0.5f, -0.5f, 0.0f),
		Vector3(-0.5f, -0.5f, 0.0f)
	};

	vertexBuffer = new VertexBuffer(vertices, 3, sizeof(Vertex));

	D3DDesc desc = D3D::GetDesc();
	CameraOption option;
	option.Width = desc.Width;
	option.Height = desc.Height;

	freedomCam = new Freedom(option);

	Context::Get()->SetMainCamera(freedomCam);
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
	Context::Get()->GetMainCamera()->GetVPBuffer()->SetVSBuffer(0);
	vertexBuffer->Render();

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	shader->Render();
	D3D::GetDC()->Draw(3,0);
}

void TestScene::PostRender()
{
}
