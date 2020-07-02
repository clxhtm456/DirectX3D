#include "Framework.h"
#include "Render2D.h"

Render2D::Render2D()
{
	
}

Render2D* Render2D::Create()
{
	auto pRet = new Render2D();
	if (pRet && pRet->Init())
	{
		pRet->AutoRelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

bool Render2D::Init()
{
	shader = Shader::Add(L"Render2D");

	D3DDesc desc = D3D::GetDesc();

	vpDesc.View = XMMatrixLookAtLH(XMVectorSet(0, 0, -1, 0), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0));
	//vpDesc.View = XMMatrixTranspose(vpDesc.View);

	vpDesc.Projection = XMMatrixOrthographicOffCenterLH(0, desc.Width, 0, desc.Height, -1, 1);
	vpDesc.Projection = XMMatrixTranspose(vpDesc.Projection);


	vpBuffer = new ConstantBuffer(&vpDesc, sizeof(ViewProjectionDesc));


	VertexTexture vertices[6];
	vertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[3].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[4].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[5].Position = Vector3(+0.5f, +0.5f, 0.0f);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 1);
	vertices[4].Uv = Vector2(0, 0);
	vertices[5].Uv = Vector2(1, 0);

	vertexBuffer = new VertexBuffer(vertices, 6, sizeof(VertexTexture));

	return true;
}

Render2D::~Render2D()
{
	delete vertexBuffer;
	delete vpBuffer;
}

void Render2D::Update()
{
	CalcWorldMatrix();
}

void Render2D::Render(Camera* viewer)
{
	Super::Render(viewer);
}

void Render2D::Draw(Camera * viewer)
{
	//VPSet();
	vpBuffer->SetVSBuffer(2);
	WorldSet();
	LightSet();

	vertexBuffer->Render();
	shader->Render();
	if (diffuseMap != NULL)
	{
		D3D::GetDC()->PSSetShaderResources(0, 1, &diffuseMap);
		D3D::GetDC()->PSSetSamplers(0, 1, &diffuseSampler);
	}

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->Draw(6, 0);
}

void Render2D::SetSRV(ID3D11ShaderResourceView * srv)
{
	diffuseMap = srv;
}
