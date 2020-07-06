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
	worldBuffer = new WorldBuffer();


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
	delete worldBuffer;
}

void Render2D::Update()
{
	Matrix S, T;
	S = XMMatrixScaling(XMVectorGetX(_scale), XMVectorGetY(_scale), XMVectorGetZ(_scale));
	T = XMMatrixTranslation(XMVectorGetX(_position), XMVectorGetY(_position), XMVectorGetZ(_position));

	_world = S * T;
	worldBuffer->SetWorld(_world);
}

void Render2D::Render(Camera* viewer)
{
}

void Render2D::Draw(Camera * viewer)
{
	//VPSet();
	vpBuffer->SetVSBuffer(2);
	worldBuffer->SetVSBuffer(1);

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

void Render2D::PostUpdate()
{
}

void Render2D::LateUpdate()
{
}


void Render2D::PostRender(Camera* viewer)
{
}
