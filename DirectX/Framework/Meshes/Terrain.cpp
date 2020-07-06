#include "Framework.h"
#include "Terrain.h"

using namespace DirectX::TriangleTests;

Terrain::Terrain()
	: RenderingNode()
{
	brushBuffer = new BrushBuffer();
	lineBuffer = new LineBuffer();
}

Terrain * Terrain::Create(UINT horizontal, UINT vertical)
{
	auto pRet = new Terrain();
	if (pRet && pRet->Init(horizontal, vertical))
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

bool Terrain::Init(UINT horizontal, UINT vertical)
{
	shader = Shader::Add(L"Terrain");
	//heightMap = Texture::Add(heightFile);

	width = horizontal;
	height = vertical;

	CreateVertexData();
	CreateIndexData();
	CreateNormalData();

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(TerrainVertex), 0, true);
	indexBuffer = new IndexBuffer(indices, indexCount);
	return true;
}

Terrain::~Terrain()
{
	SafeDeleteArray(vertices);
	SafeDelete(vertexBuffer);

	SafeDeleteArray(indices);
	SafeDelete(indexBuffer);

	SafeDelete(brushBuffer);
	SafeDelete(lineBuffer);
}

void Terrain::Update()
{	
	Super::Update();	

	if (brushBuffer->data.Type > 0)
	{
		brushBuffer->data.Location = GetPickedPosition();

		if (Mouse::Get()->Press(0))
			RaiseHeight(brushBuffer->data.Location, brushBuffer->data.Type, brushBuffer->data.Range);
	}
}

void Terrain::Render(Camera* viewer)
{
	Super::Render(viewer);

	if (baseMap != NULL)
		baseMap->Set(0);

	brushBuffer->SetVSBuffer(2);
	lineBuffer->SetVSBuffer(3);

	vertexBuffer->Render();
	indexBuffer->Render();

	shader->Render();
	if (layerMap != NULL && alphaMap != NULL)
	{
		layerMap->Set(1);
		alphaMap->Set(2);
	}
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);

}

void Terrain::BaseMap(wstring file)
{
	baseMap = Texture::Add(file);
}

void Terrain::LayerMap(wstring file, wstring alpha)
{
	alphaMap = Texture::Add(file);
	layerMap = Texture::Add(file);
}

float Terrain::GetHeight(Vector3& position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + (x + 1);
	index[3] = width * (z + 1) + (x + 1);

	XMVECTOR v[4];
	for (int i = 0; i < 4; i++)
		v[i] = XMLoadFloat3(&vertices[index[i]].Position);

	float ddx = (position.x - XMVectorGetX(v[0])) / 1.0f;
	float ddz = (position.z - XMVectorGetZ(v[0])) / 1.0f;

	XMVECTOR result;

	if(ddx + ddz <= 1)
		result = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	else
	{
		ddx = 1 - ddx;
		ddz = 1 - ddz;
		result = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}

	return XMVectorGetY(result);

}

float Terrain::GetHeightPick(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + (x + 1);
	index[3] = width * (z + 1) + (x + 1);

	XMVECTOR p[4];
	for (int i = 0; i < 4; i++)
		p[i] = XMLoadFloat3(&vertices[index[i]].Position);

	XMVECTOR start = XMVectorSet(position.x, 1000, position.z,0);
	XMVECTOR direction = XMVectorSet(0, -1, 0,0);

	float u, v, distance;
	XMVECTOR result = XMVectorSet(-1, FLT_MIN, -1,0);
	if(Intersects(start, direction,p[0], p[1], p[2], distance) == TRUE)
		result = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

	if (Intersects(start, direction,p[3], p[1], p[2], distance) == TRUE)
		result = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

	return XMVectorGetY(result);
}

Vector3 Terrain::GetPickedPosition()
{
	Matrix V = Context::Get()->GetMainCamera()->ViewMatrix();
	Matrix P = Context::Get()->GetMainCamera()->ProjectionMatrix();
	Viewport* vp = Context::Get()->GetMainCamera()->GetViewport();

	Vector3 mouse = Mouse::Get()->GetPosition();

	Vector3 n, f;

	mouse.z = 0.0f;
	vp->UnProject(&n, mouse, GetWorld(), V, P);

	mouse.z = 1.0f;
	vp->UnProject(&f, mouse, GetWorld(), V, P);

	XMVECTOR start = XMLoadFloat3(&n);
	XMVECTOR direction = XMLoadFloat3(&f) - XMLoadFloat3(&n);

	XMVECTOR result = XMVectorSet(-1, FLT_MIN, -1,0);
	Vector3 fResult;
	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{			
			UINT index[4];
			index[0] = width * z + x;
			index[1] = width * (z + 1) + x;
			index[2] = width * z + (x + 1);
			index[3] = width * (z + 1) + (x + 1);

			XMVECTOR p[4];
			for (int i = 0; i < 4; i++)
				p[i] = XMLoadFloat3(&vertices[index[i]].Position);

			float u, v, distance;

			if (Intersects(start, direction, p[0], p[1], p[2], distance) == TRUE)
			{
				result = (p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v);
				break;
			}

			if (Intersects(start, direction, p[3], p[1], p[2], distance) == TRUE)
			{
				result = (p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v);
				break;
			}
		}
	}
	XMStoreFloat3(&fResult, result);
	return fResult;
}

void Terrain::SetHeight(float x, float z, float height)
{
}

void Terrain::CreateVertexData()
{
	//heightMap의 색상 정보 받아오기
	//vector<Vector4> heights = heightMap->ReadPixels();

	//Create VData
	vertexCount = width * height;
	vertices = new TerrainVertex[vertexCount];

	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * z + x;
			UINT pixel = width * (height - z - 1) + x;

			vertices[index].Position.x = (float)x;
			//vertices[index].Position.y = (heights[pixel].x * 256.0f) / 10.0f;
			vertices[index].Position.y = 0;
			vertices[index].Position.z = (float)z;

			vertices[index].Uv.x = (float)x / (float)width;
			vertices[index].Uv.y = (float)(height - 1 - z) / (float)height;
		}
	}
}

void Terrain::CreateIndexData()
{
	indexCount = (width - 1) * (height - 1) * 6;
	indices = new UINT[indexCount];

	UINT index = 0;
	for (UINT y = 0; y < height - 1; y++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			indices[index + 0] = width * y + x;
			indices[index + 1] = width * (y + 1) + x;
			indices[index + 2] = width * y + (x + 1);
			indices[index + 3] = width * y + (x + 1);
			indices[index + 4] = width * (y + 1) + x;
			indices[index + 5] = width * (y + 1) + (x + 1);

			index += 6;
		}
	}
}

void Terrain::CreateNormalData()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		TerrainVertex v0 = vertices[index0];
		TerrainVertex v1 = vertices[index1];
		TerrainVertex v2 = vertices[index2];

		XMVECTOR d1 = XMLoadFloat3(&v1.Position) - XMLoadFloat3(&v0.Position);
		XMVECTOR d2 = XMLoadFloat3(&v2.Position) - XMLoadFloat3(&v0.Position);

		XMVECTOR normal;
		normal = XMVector3Cross(d1, d2);
		
		XMStoreFloat3(&vertices[index0].Normal, normal);
		XMStoreFloat3(&vertices[index1].Normal, normal);
		XMStoreFloat3(&vertices[index2].Normal, normal);	
	}

	for (UINT i = 0; i < vertexCount; i++)
	{
		XMVECTOR temp = XMLoadFloat3(&vertices[i].Normal);
		temp = XMVector3Normalize(temp);
		XMStoreFloat3(&vertices[i].Normal, temp);
	}
}

void Terrain::RaiseHeight(Vector3 & position, UINT type, UINT range)
{
	D3D11_BOX rect;
	rect.left = (LONG)position.x - range;
	rect.top = (LONG)position.z + range;
	rect.right = (LONG)position.x + range;
	rect.bottom = (LONG)position.z - range;

	if (rect.left < 0) rect.left = 0;
	if (rect.right >= width) rect.right = width;
	if (rect.bottom < 0) rect.bottom = 0;
	if (rect.top >= height) rect.top = height;

	for (LONG z = rect.bottom; z <= rect.top; z++)
	{
		for (LONG x = rect.left; x <= rect.right; x++)
		{
			UINT index = width * (UINT)z + (UINT)x;
			vertices[index].Position.y += 5.0f * Time::Delta();
		}
	}

	CreateNormalData();

	/*D3D::GetDC()->UpdateSubresource
	(
		vertexBuffer->Buffer(), 0, NULL, vertices, sizeof(TerrainVertex) * vertexCount, 0
	);*/
	
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, vertices, sizeof(TerrainVertex) * vertexCount);
	}
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
}

///과제
// 1. BrushType -> Sphere Type 추가
// 2. Shift키를 누르고 클릭하면 지면 하강
// 3. Noise(랜덤)
// 4. Smooth(평균화)
// 5. Flat(평탄화)
// 6. Slope(두 점사이의 경사)