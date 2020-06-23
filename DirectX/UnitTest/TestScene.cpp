#include "stdafx.h"
#include "TestScene.h"
#include "Viewer/Freedom.h"

void TestScene::Initialize()
{
	shader = Shader::Add(L"Vertex");
	Vertex vertices[] =
	{
		Vector3(-0.5f, -0.5f, 0.0f),
		Vector3(-0.5f, +0.5f, 0.0f),
		Vector3(+0.5f, -0.5f, 0.0f),
		Vector3(+0.5f, +0.5f, 0.0f)
	};

	UINT indices[] =
	{
		0,
		1,
		2,
		2,
		1,
		3
	};
	
	vertexBuffer = new VertexBuffer(vertices, 4, sizeof(Vertex));
	indexBuffer = new IndexBuffer(indices, 6);

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
	delete vertexBuffer;
}

void TestScene::Update()
{
}

void TestScene::PreRender()
{
}

void TestScene::Render()
{
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	vertexBuffer->Render();
	indexBuffer->Render();

	shader->Render();
	Context::Get()->GetMainCamera()->GetVPBuffer()->SetVSBuffer(0);
	D3D::GetDC()->DrawIndexed(6,0,0);
}

void TestScene::PostRender()
{
}
