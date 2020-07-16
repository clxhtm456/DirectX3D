#include "Framework.h"
#include "Terrain.h"
#include "QuadTree.h"

#include <d3dx10math.h>

#pragma comment(lib, "d3dx10.lib")

using namespace DirectX::TriangleTests;

/*
TerrainTexture 
출처 : https://copynull.tistory.com/307?category=649931
*/

Terrain::Terrain()
	: RenderingNode()
{
}

Terrain * Terrain::Create(UINT horizontal, UINT vertical, UINT textureDetail)
{
	auto pRet = new Terrain();
	if (pRet && pRet->Init(horizontal, vertical, textureDetail))
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

bool Terrain::Init(UINT horizontal, UINT vertical, UINT textureDetail)
{
	shader = Shader::Add(L"Terrain");
	//heightMap = Texture::Add(heightFile);

	width = horizontal;
	height = vertical;
	detail = textureDetail;

	LoadHeightMap("");

	CalculateNormals();
	CalculateTextureCoordinate();
	InitializeBuffers();

	brushBuffer = new ConstantBuffer(&brushDesc, sizeof(BrushDesc));
	lineBuffer = new ConstantBuffer(&lineDesc, sizeof(LineDesc));

	m_QuadTree = new QuadTree(this);

	// 텍스처 샘플러 상태 구조체를 설정합니다.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 텍스처 샘플러 상태를 만듭니다.
	HRESULT hr = D3D::GetDevice()->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

Terrain::~Terrain()
{
	SafeDeleteArray(vertices);
	SafeDelete(vertexBuffer);

	SafeDeleteArray(indices);
	SafeDelete(indexBuffer);

	delete _heightMap;
	delete brushBuffer;
	delete lineBuffer;

	delete m_QuadTree;

	m_sampleState->Release();
}

void Terrain::Update()
{	
	Super::Update();	

	ImGui::Text("Type");

	int brushSize = sizeof(BrushType) / sizeof(bool);
	for (int i = 0; i < brushSize; i++)
	{
		if (ImGui::Checkbox(BrushName[i], &BrushType[i]))
		{
			if (BrushType[i] == false)
				BrushType[i] = true;
			else
			{
				for (int t = 0; t < brushSize; t++)
				{
					if (t != i)
						BrushType[t] = false;
				}
				brushDesc.Type = i;
			}
		}
		//ImGui::SameLine();
	}

	ImGui::Text("Style");

	brushSize = sizeof(BrushStyle) / sizeof(bool);
	for (int i = 0; i < brushSize; i++)
	{
		if (ImGui::Checkbox(BrushStyleName[i], &BrushStyle[i]))
		{
			if (BrushStyle[i] == false)
				BrushStyle[i] = true;
			else
			{
				for (int t = 0; t < brushSize; t++)
				{
					if (t != i)
						BrushStyle[t] = false;
				}
				IBrushStyle = i;
			}
		}
		//ImGui::SameLine();
	}

	ImGui::InputInt("Range", (int*)&brushDesc.Range);
	ImGui::Separator();
	brushDesc.Range %= 20;

	string str = "MousePos : " + to_string(brushDesc.Location.x) + to_string(brushDesc.Location.y) + to_string(brushDesc.Location.z);
	Gui::Get()->RenderText(5, 50, 1, 1, 1, str);

	if (brushDesc.Type > 0)
	{
		float mouseDiff = sqrt(Mouse::Get()->GetMoveValue().x* Mouse::Get()->GetMoveValue().x + Mouse::Get()->GetMoveValue().y* Mouse::Get()->GetMoveValue().y);
		if (mouseDiff > 0.1f)
			brushDesc.Location = GetPickedPosition();//GetPickedPosition 에 의해 프레임감소

		vector<VertexTextureNormal*> vertexVector;
		switch (brushDesc.Type)
		{
		case 1:
			vertexVector = SqureArea(brushDesc.Location, brushDesc.Type, brushDesc.Range);
			break;
		case 2:
			vertexVector = CircleArea(brushDesc.Location, brushDesc.Type, brushDesc.Range);
			break;
		}

		if (Mouse::Get()->Press(0))
		{
			switch (IBrushStyle)
			{
			case 0:
				RaiseHeight(vertexVector, 5.0f);
				break;
			case 1:
				FallHeight(vertexVector, 5.0f);
				break;
			case 2:
				NoiseHeight(vertexVector, -2.0f, 2.0f);
				break;
			case 3:
				SmoothHeight(vertexVector, 3.0f, 5.0f);
				break;
			case 4:
				FlatHeight(vertexVector, 5.0f);
				break;
			case 5:
				break;
			default:
				break;
			}
		}

		if (Mouse::Get()->Down(0))
		{
			if (IBrushStyle == 5)
				SlopeHeight(vertexVector);
		}
	}

	ImGui::Separator();

	ImGui::InputInt("visible", (int*)&lineDesc.Visible);
	lineDesc.Visible %= 2;

	ImGui::InputFloat("thickness", &lineDesc.Thickness, 0.001f);
	Math::Clamp(lineDesc.Thickness, 0.01f, 0.9f);

	ImGui::InputFloat("Size", &lineDesc.Size);

}

void Terrain::ResourceBinding(Camera* viewer)
{
	if (baseMap != NULL)
		baseMap->Set(0);

	brushBuffer->SetPSBuffer(3);
	lineBuffer->SetPSBuffer(4);

	VPSet(viewer);
	WorldSet();

	LightSet();

}

void Terrain::Render(Camera * viewer)
{
	/*if (vertexBuffer != NULL && indexBuffer != NULL)
	{
		vertexBuffer->Render();
		indexBuffer->Render();
		rasterizerState->SetState();

		if (vsShader == NULL && psShader == NULL)
			shader->Render();
		else
		{
			vsShader->RenderVS();
			psShader->RenderPS();
		}

		D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	}*/

	m_QuadTree->Render(viewer);
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
	Matrix V = Context::Get()->GetMainCamera()->GetViewMatrix();
	D3DXMATRIX Vx;
	for (int i = 0; i < 4; i++)
	{
		Vector4 tempV;
		XMStoreFloat4(&tempV, V.r[i]);
		Vx.m[i][0] = tempV.x;
		Vx.m[i][1] = tempV.y;
		Vx.m[i][2] = tempV.z;
		Vx.m[i][3] = tempV.w;
	}
	Matrix P = Context::Get()->GetMainCamera()->GetProjectionMatrix();
	D3DXMATRIX Px;
	for (int i = 0; i < 4; i++)
	{
		Vector4 tempP;
		XMStoreFloat4(&tempP, P.r[i]);
		Px.m[i][0] = tempP.x;
		Px.m[i][1] = tempP.y;
		Px.m[i][2] = tempP.z;
		Px.m[i][3] = tempP.w;
	}
	Viewport* vp = Context::Get()->GetMainCamera()->GetViewport();

	Vector3 mouse = Mouse::Get()->GetPosition();
	D3DXVECTOR3 mousex;
	mousex.x = mouse.x;
	mousex.y = mouse.y;
	mousex.z = mouse.z;

	Vector3 n, f;

	mouse.z = 0.0f;
	vp->UnProject(&n, mouse, GetWorld(), V, P);

	mouse.z = 1.0f;
	vp->UnProject(&f, mouse, GetWorld(), V, P);

	D3DXVECTOR3 nx;
	nx.x = n.x;
	nx.y = n.y;
	nx.z = n.z;

	D3DXVECTOR3 fx;
	fx.x = f.x;
	fx.y = f.y;
	fx.z = f.z;

	D3DXVECTOR3 start;
	start.x= n.x;
	start.y = n.y;
	start.z = n.z;
	D3DXVECTOR3 direction = fx - nx;
	D3DXVec3Normalize(&direction, &direction);

	XMVECTOR result = XMVectorSet(-1, FLT_MIN, -1,0);

	for (int index = 0; index < indexCount; index+=6)
	{
		D3DXVECTOR3 p[6];
		for (int i = 0; i < 6; i++)
		{
			p[i].x = vertices[index + i].Position.x;
			p[i].y = vertices[index + i].Position.y;
			p[i].z = vertices[index + i].Position.z;
		}

		float u, v, distance;
		if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		{
			D3DXVECTOR3 temp = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;
			Vector3 result;
			result.x = temp.x;
			result.y = temp.y;
			result.z = temp.z;


			return result;
		}

		if (D3DXIntersectTri(&p[3], &p[4], &p[5], &start, &direction, &u, &v, &distance) == TRUE)
		{
			D3DXVECTOR3 temp = p[3] + (p[4] - p[3]) * u + (p[5] - p[3]) * v;
			Vector3 result;
			result.x = temp.x;
			result.y = temp.y;
			result.z = temp.z;
			return result;
		}
	}

	return Vector3(-1, FLT_MIN, -1);

	/*Matrix V = Context::Get()->GetMainCamera()->GetViewMatrix();
	Matrix P = Context::Get()->GetMainCamera()->GetProjectionMatrix();
	Viewport* vp = Context::Get()->GetMainCamera()->GetViewport();

	Vector3 mouse = Mouse::Get()->GetPosition();

	Vector3 n, f;

	mouse.z = 0.0f;
	vp->UnProject(&n, mouse, GetWorld(), V, P);

	mouse.z = 1.0f;
	vp->UnProject(&f, mouse, GetWorld(), V, P);

	XMVECTOR start = XMLoadFloat3(&n);
	XMVECTOR direction = XMLoadFloat3(&f) - XMLoadFloat3(&n);
	direction = XMVector3Normalize(direction);

	XMVECTOR result = XMVectorSet(-1, FLT_MIN, -1, 0);
	Vector3 fResult;
	for (int index = 0; index < indexCount; index += 6)
	{
		XMVECTOR p[6];
		for (int i = 0; i < 6; i++)
		{
			p[i] = XMLoadFloat3(&vertices[index+i].Position);
		}
		float distance;
		if (Intersects(start, direction, p[0], p[1], p[2], distance) == TRUE)
		{
			result = (p[0] + (p[1] - p[0]) + (p[2] - p[0]));
			break;
		}

		if (Intersects(start, direction, p[3], p[1], p[2], distance) == TRUE)
		{
			result = (p[3] + (p[1] - p[3]) + (p[2] - p[3]));
			break;
		}
	}

	XMStoreFloat3(&fResult, result);
	return fResult;*/
}

void Terrain::SetHeight(float x, float z, float height)
{
}

void Terrain::CopyVertexArray(void * vertexList)
{
	memcpy(vertexList, vertices, sizeof(VertexTextureNormal) * vertexCount);
}

bool Terrain::LoadHeightMap(const char * filename)
{
	////파일 불러오기
	//{
	//	FILE* filePtr = nullptr;
	//	if (fopen_s(&filePtr, filename, "rb") != 0)
	//		return false;
	//}

	_heightMap = new HeightMapType[width * height];
	
	if (!_heightMap)
		return false;

	int k = 0;
	
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			//UCHAR bitMapheight = bitmapimage[k];

			int index = height * j + i;

			_heightMap[index].x = (float)i;
			_heightMap[index].y = (float)0;
			_heightMap[index].z = (float)j;

			k += 3;
		}
	}

	//비트맵 이미지 데이터 해제
	{

	}

	return true;

}

bool Terrain::CalculateNormals()
{
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index = 0;
	int count = 0;
	float vertex1[3] = { 0.0f,0.0f,0.0f };
	float vertex2[3] = { 0.0f,0.0f,0.0f };
	float vertex3[3] = { 0.0f,0.0f,0.0f };
	float vector1[3] = { 0.0f,0.0f,0.0f };
	float vector2[3] = { 0.0f,0.0f,0.0f };
	float sum[3] = { 0.0f,0.0f,0.0f };
	float length = 0.0f;

	VectorType* normals = new VectorType[(height - 1) * (width - 1)];
	if (!normals)
		return false;
	for (int j = 0; j < height - 1; j++)
	{
		for (int i = 0; i < width - 1; i++)
		{
			index1 = (j*height) + i;
			index2 = (j*height) + (i +1);
			index3 = ((j+1)*height) + i;
			
			vertex1[0] = _heightMap[index1].x;
			vertex1[1] = _heightMap[index1].y;
			vertex1[2] = _heightMap[index1].z;

			vertex2[0] = _heightMap[index2].x;
			vertex2[1] = _heightMap[index2].y;
			vertex2[2] = _heightMap[index2].z;

			vertex3[0] = _heightMap[index3].x;
			vertex3[1] = _heightMap[index3].y;
			vertex3[2] = _heightMap[index3].z;

			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j*(height - 1)) + i;

			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	//각 정점의 각면의 평균
	//정점이 닿아 그 정점에 대한 평균값을 얻어옴
	for (int j = 0; j < height - 1; j++)
	{
		for (int i = 0; i < width - 1; i++)
		{
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			count = 0;

			//왼쪽아래면
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1)*(height - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			//오른쪽아래면
			if ((i < (width -1)) && ((j-1) >= 0))
			{
				index = ((j - 1)*(height - 1)) + (i);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			//왼쪽위면
			if (((i - 1) >= 0) && (j<(height -1)))
			{
				index = (j*(height - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			//오른쪽위면
			if ((i < (width - 1)) && (j < (height - 1)))
			{
				index = (j*(height - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			sum[0] = (sum[0]/(float)count);
			sum[1] = (sum[1]/(float)count);
			sum[2] = (sum[2]/(float)count);

			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			index = (j * height) + i;

			_heightMap[index].nx = (sum[0] / length);
			_heightMap[index].ny = (sum[1] / length);
			_heightMap[index].nz = (sum[2] / length);
		}
	}
	delete[] normals;
	normals = 0;

	return true;

}

bool Terrain::InitializeBuffers()
{
	float tu = 0.0f;
	float tv = 0.0f;

	vertexCount = (width - 1) *(height - 1) * 6;
	indexCount = vertexCount;

	vertices = new VertexTextureNormal[vertexCount];
	if (!vertices)
		return false;

	indices = new UINT[indexCount];
	if (!indices)
		return false;

	int index = 0;

	for (int j = 0; j < height - 1; j++)
	{
		for (int i = 0; i < width - 1; i++)
		{
			int index1 = (height * j) + (i);
			int index2= (height * j) + (i+1);
			int index3 = (height * (j+1)) + (i);
			int index4 = (height * (j+1)) + (i+1);

			//0,1 왼쪽 위
			{
				tv = _heightMap[index3].tv;

				if (tv == 1.0f)
				{
					tv = 0.0f;
				}

				vertices[index].Position = XMFLOAT3(_heightMap[index3].x, _heightMap[index3].y, _heightMap[index3].z);
				vertices[index].Uv = XMFLOAT2(_heightMap[index3].tu, tv);
				vertices[index].Normal = XMFLOAT3(_heightMap[index3].nx, _heightMap[index3].ny, _heightMap[index3].nz);

				indices[index] = index;
				index++;
			}
			
			//1,1 오른쪽 위 
			{
				tu = _heightMap[index4].tu;
				tv = _heightMap[index4].tv;

				if (tu == 0.0f)
					tu = 1.0f;
				if (tv == 1.0f)
					tv = 0.0f;

				vertices[index].Position = XMFLOAT3(_heightMap[index4].x, _heightMap[index4].y, _heightMap[index4].z);
				vertices[index].Uv = XMFLOAT2(tu, tv);
				vertices[index].Normal = XMFLOAT3(_heightMap[index4].nx, _heightMap[index4].ny, _heightMap[index4].nz);
				indices[index] = index;
				index++;
			}
			
			//0,0 왼쪽 아래
			{
				vertices[index].Position = XMFLOAT3(_heightMap[index1].x, _heightMap[index1].y, _heightMap[index1].z);
				vertices[index].Uv = XMFLOAT2(_heightMap[index1].tu, _heightMap[index1].tv);
				vertices[index].Normal = XMFLOAT3(_heightMap[index1].nx, _heightMap[index1].ny, _heightMap[index1].nz);
				indices[index] = index;
				index++;
			}

			//0,0 왼쪽 아래
			{
				vertices[index].Position = XMFLOAT3(_heightMap[index1].x, _heightMap[index1].y, _heightMap[index1].z);
				vertices[index].Uv = XMFLOAT2(_heightMap[index1].tu, _heightMap[index1].tv);
				vertices[index].Normal = XMFLOAT3(_heightMap[index1].nx, _heightMap[index1].ny, _heightMap[index1].nz);
				indices[index] = index;
				index++;
			}

			//1,1 오른쪽 위
			{
				tu = _heightMap[index4].tu;
				tv = _heightMap[index4].tv;

				if (tu == 0.0f)
					tu = 1.0f;
				if (tv == 1.0f)
					tv = 0.0f;

				vertices[index].Position = XMFLOAT3(_heightMap[index4].x, _heightMap[index4].y, _heightMap[index4].z);
				vertices[index].Uv = XMFLOAT2(tu, tv);
				vertices[index].Normal = XMFLOAT3(_heightMap[index4].nx, _heightMap[index4].ny, _heightMap[index4].nz);
				indices[index] = index;
				index++;
			}
			
			//1,0 오른쪽 아래
			{
				tu = _heightMap[index2].tu;

				if (tu == 0.0f)
					tu = 1.0f;

				vertices[index].Position = XMFLOAT3(_heightMap[index2].x, _heightMap[index2].y, _heightMap[index2].z);
				vertices[index].Uv = XMFLOAT2(tu, _heightMap[index2].tv);
				vertices[index].Normal = XMFLOAT3(_heightMap[index2].nx, _heightMap[index2].ny, _heightMap[index2].nz);
				indices[index] = index;
				index++;
			}
		}
	}


	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexTextureNormal), 0, true);
	indexBuffer = new IndexBuffer(indices, indexCount);

	return true;
}


void Terrain::CalculateTextureCoordinate()
{
	//텍스쳐 좌표 증가량
	float incrementValue = (float)detail / (float)width;

	//텍스쳐 반복횟수
	int incrementCount = width / detail;

	float tuCoordinate = 0.0f;
	float tvCoordinate = 1.0f;

	int tuCount = 0;
	int tvCount = 0;

	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * z + x;

			_heightMap[index].tu = tuCoordinate;
			_heightMap[index].tv = tvCoordinate;

			tuCoordinate += incrementValue;
			tuCount++;

			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		tvCoordinate -= incrementValue;
		tvCount++;

		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}
}

void Terrain::ReDrawNormal()
{
	/*for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		VertexTextureNormal v0 = vertices[index0];
		VertexTextureNormal v1 = vertices[index1];
		VertexTextureNormal v2 = vertices[index2];

		XMVECTOR d1 = XMLoadFloat3(&v1.Position) - XMLoadFloat3(&v0.Position);
		XMVECTOR d2 = XMLoadFloat3(&v2.Position) - XMLoadFloat3(&v0.Position);

		XMVECTOR normal = XMVector3Cross(d1, d2);
		Vector3 vNormal;
		XMStoreFloat3(&vNormal, normal);

		vertices[index0].Normal = vNormal;
		vertices[index1].Normal = vNormal;
		vertices[index2].Normal = vNormal;
	}*/

	/*for (UINT i = 0; i < vertexCount; i++)
	{
		XMVECTOR temp = XMLoadFloat3(&vertices[i].Normal);
		temp = XMVector3Normalize(temp);
		XMStoreFloat3(&vertices[i].Normal, temp);
	}*/

	/*D3D::GetDC()->UpdateSubresource
	(
		vertexBuffer->Buffer(), 0, NULL, vertices, sizeof(VertexTextureNormal) * vertexCount, 0
	);*/

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, vertices, sizeof(VertexTextureNormal) * vertexCount);
	}
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
}

void Terrain::RaiseHeight(vector<VertexTextureNormal*> vertexVector, float speed)
{
	for (auto vertex : vertexVector)
	{
		vertex->Position.y += speed * Time::Delta();
	}

	ReDrawNormal();
}

void Terrain::FallHeight(vector<VertexTextureNormal*> vertexVector, float speed)
{
	for (auto vertex : vertexVector)
	{
		vertex->Position.y -= speed * Time::Delta();
	}

	ReDrawNormal();
}

void Terrain::NoiseHeight(vector<VertexTextureNormal*> vertexVector, float min, float max)
{
	for (auto vertex : vertexVector)
	{
		vertex->Position.y = Math::Random(min, max);
	}

	ReDrawNormal();
}

void Terrain::SmoothHeight(vector<VertexTextureNormal*> vertexVector, float value, float speed)
{
	for (auto vertex : vertexVector)
	{
		if (vertex->Position.y - value > 0.1f)
			vertex->Position.y -= speed * Time::Delta();
		else if (vertex->Position.y - value < 0.1f)
			vertex->Position.y += speed * Time::Delta();
	}

	ReDrawNormal();
}

void Terrain::FlatHeight(vector<VertexTextureNormal*> vertexVector, float speed)
{
	float value = 0;

	for (auto vertex : vertexVector)
	{
		value += vertex->Position.y;
	}
	value /= vertexVector.size();

	for (auto vertex : vertexVector)
	{
		if (vertex->Position.y - value > 0.1f)
			vertex->Position.y -= speed * Time::Delta();
		else if (vertex->Position.y - value < 0.1f)
			vertex->Position.y += speed * Time::Delta();
	}

	ReDrawNormal();
}

void Terrain::SlopeHeight(vector<VertexTextureNormal*> vertexVector)
{
	if (slopeVector.size() == 0)
	{
		for (auto vertex : vertexVector)
		{
			slopeVector.push_back(vertex);
		}
	}
	else
	{
		//계산식
		for (int i = 0; i < vertexVector.size(); i++)
		{
			float totaldx = slopeVector[i]->Position.x - vertexVector[i]->Position.x;
			float totaldz = slopeVector[i]->Position.z - vertexVector[i]->Position.z;
			float totaldy = slopeVector[i]->Position.y - vertexVector[i]->Position.y;
			float totaldist = sqrt(totaldx * totaldx + totaldz * totaldz);

			float lean = totaldz / totaldx;
			float startPosX = totaldx > 0 ? vertexVector[i]->Position.x : slopeVector[i]->Position.x;
			float zValue = totaldx > 0 ? vertexVector[i]->Position.z : slopeVector[i]->Position.z;
			float endPosX = totaldx > 0 ? slopeVector[i]->Position.x : vertexVector[i]->Position.x;
			float leanPos = totaldx > 0 ? startPosX : endPosX;

			for (int x = startPosX; x <= endPosX; x++)
			{
				int z = lean * (x - startPosX) + zValue;
				UINT index = width * (UINT)z + (UINT)x;
				float dx = leanPos - vertices[index].Position.x;
				float dz = (lean * (leanPos - startPosX) + zValue) - vertices[index].Position.z;
				float dist = sqrt(dx * dx + dz * dz);
				vertices[index].Position.y = totaldy * dist / totaldist;
			}
		}
		slopeVector.clear();

		ReDrawNormal();
	}
}

vector<VertexTextureNormal*> Terrain::SqureArea(Vector3 position, UINT type, UINT range)
{
	vector<VertexTextureNormal*> vertexVector;
	D3D11_BOX rect;
	rect.left = (LONG)position.x - range;
	rect.right = (LONG)position.x + range;
	rect.bottom = (LONG)position.z - range;
	rect.top = (LONG)position.z + range;

	if (rect.left < 0) rect.left = 0;
	if (rect.right > width) rect.right = width;

	if (rect.bottom < 0) rect.bottom = 0;
	if (rect.top > height) rect.top = height;


	for (LONG z = rect.bottom; z < rect.top; z++)
	{
		for (LONG x = rect.left; x < rect.right; x++)
		{
			UINT index = width * (UINT)z + (UINT)x;
			vertexVector.push_back(&vertices[index]);
		}
	}

	return vertexVector;
}

vector<VertexTextureNormal*> Terrain::CircleArea(Vector3 position, UINT type, UINT range)
{
	vector<VertexTextureNormal*> vertexVector;
	D3D11_BOX rect;
	rect.left = (LONG)position.x - range;
	rect.right = (LONG)position.x + range;
	rect.bottom = (LONG)position.z - range;
	rect.top = (LONG)position.z + range;

	if (rect.left < 0) rect.left = 0;
	if (rect.right > width) rect.right = width;

	if (rect.bottom < 0) rect.bottom = 0;
	if (rect.top > height) rect.top = height;

	for (LONG z = rect.bottom; z < rect.top; z++)
	{
		for (LONG x = rect.left; x < rect.right; x++)
		{
			UINT index = width * (UINT)z + (UINT)x;
			float dx = position.x - vertices[index].Position.x;
			float dz = position.z - vertices[index].Position.z;
			float dist = sqrt(dx * dx + dz * dz);
			if (dist > range)
				continue;
			vertexVector.push_back(&vertices[index]);
		}
	}
	return vertexVector;
}

///과제
// 1. BrushType -> Sphere Type 추가
// 2. Shift키를 누르고 클릭하면 지면 하강
// 3. Noise(랜덤)
// 4. Smooth(평균화)
// 5. Flat(평탄화)
// 6. Slope(두 점사이의 경사)