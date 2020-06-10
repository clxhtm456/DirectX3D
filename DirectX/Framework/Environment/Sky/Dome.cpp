#include "Framework.h"
#include "Dome.h"

Dome::Dome(Shader * shader, Vector3 position, Vector3 scale, UINT drawCount):
	Renderer(shader)
{
	starMap = new Texture(L"Environment/Starfield.png");
	sStarMap = shader->AsSRV("StarMap");

	GetTransform()->Position(position);
	GetTransform()->Scale(scale);

	UINT latitude = drawCount * 0.5f;//위도
	UINT longitude = drawCount;//경도

	vertexCount = longitude * latitude * 2;
	indexCount = (longitude - 1)  * (latitude - 1) * 2 * 8;

	VertexTexture* vertices = new VertexTexture[vertexCount];

	UINT index = 0;
	for (UINT i = 0; i < longitude; i++)
	{
		float xz = Math::ToRadian(100.0f * (i / (longitude - 1.0f)));

		for (UINT j = 0; j < latitude; j++)
		{
			float y = Math::PI * j / (latitude - 1);

			vertices[index].Position.x = sinf(xz) * cosf(y);
			vertices[index].Position.y = cosf(xz);
			vertices[index].Position.z = sinf(xz) * sinf(y);

			vertices[index].Uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].Uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		}
	}

	for (UINT i = 0; i < longitude; i++)
	{
		float xz = Math::ToRadian(100.0f * (i / (longitude - 1.0f)));

		for (UINT j = 0; j < latitude; j++)
		{
			float y = (Math::PI * 2.0f) - (Math::PI * j / (latitude - 1));

			vertices[index].Position.x = sinf(xz) * cosf(y);
			vertices[index].Position.y = cosf(xz);
			vertices[index].Position.z = sinf(xz) * sinf(y);

			vertices[index].Uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].Uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		}
	}

	index = 0; 
	UINT * indices = new UINT[indexCount * 3];
	for (UINT i = 0; i < longitude-1; i++)
	{
		for (UINT j = 0; j < latitude-1; j++)
		{
			indices[index++] = i * latitude + j;
			indices[index++] = (i + 1)*  latitude + j;
			indices[index++] = (i + 1) * latitude + (j + 1);

			indices[index++] = (i + 1) * latitude + (j + 1);
			indices[index++] = i * latitude + (j + 1);
			indices[index++] = i * latitude + j;
		}
	}
	UINT offset = latitude * longitude;
	for (UINT i = 0; i < longitude - 1; i++)
	{
		for (UINT j = 0; j < latitude - 1; j++)
		{
			indices[index++] = offset + i * latitude + j;
			indices[index++] = offset + (i + 1) * latitude + (j + 1);
			indices[index++] = offset + (i + 1) * latitude + j;

			indices[index++] = offset + i * latitude + (j + 1);
			indices[index++] = offset + (i + 1) * latitude + (j + 1);
			indices[index++] = offset + i * latitude + j;
		}
	}

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexTexture));
	indexBuffer = new IndexBuffer(indices, indexCount);

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	//DepthStencilState
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		desc.DepthEnable = false;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = true;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		//depth 검사시 카메라에 더 가까운 하늘에 의해 오브젝트는 stencil 검사에서 탈락 탈락햇을경우에도 keep하는 옵션
		const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp2 =
		{
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,
			D3D11_COMPARISON_EQUAL
		};

		const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = {
			D3D11_STENCIL_OP_REPLACE,
			D3D11_STENCIL_OP_REPLACE,
			D3D11_STENCIL_OP_REPLACE,
			D3D11_COMPARISON_ALWAYS
		};

		desc.FrontFace = stencilMarkOp;
		desc.BackFace = stencilMarkOp2;

		Check(D3D::GetDevice()->CreateDepthStencilState(&desc, &dss));
	}

	sDss = shader->AsDepthStencil("SkyDepthStencil");
}

Dome::~Dome()
{
}

void Dome::Update()
{
	Super::Update();
}

void Dome::Render()
{
	Super::Render();

	sDss->SetDepthStencilState(0, dss);

	sStarMap->SetResource(starMap->SRV());
	shader->DrawIndexed(0, Pass(), indexCount);
}
