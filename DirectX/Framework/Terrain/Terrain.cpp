#include "Framework.h"
#include "Terrain.h"
#include "QuadTree.h"
#include "TerrainCellClass.h"
#include "Viewer/Frustum.h"

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
	SetShader( Shader::Add(L"Terrain"));
	//heightMap = Texture::Add(heightFile);

	width = horizontal;
	height = vertical;
	detail = textureDetail;

	bool result = LoadSetupFile("");
	if (!result)
	{
		return false;
	}

	// 원시 파일의 데이터로 지형 높이 맵을 초기화합니다.
	result = LoadRawHeightMap();
	if (!result)
	{
		return false;
	}

	// 높이 스케일에 대한 X 및 Z 좌표를 설정하고 높이 스케일 값에 따라 지형 높이를 조정합니다.
	SetTerrainCoordinates();

	// 지형 데이터의 법선을 계산합니다.
	result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	// 지형의 컬러 맵에 로드합니다.
	result = LoadColorMap();
	if (!result)
	{
		return false;
	}

	// 이제 지형의 3D 모델을 작성하십시오.
	result = BuildTerrainModel();
	if (!result)
	{
		return false;
	}

	// 이제 3D 지형 모델이 만들어지면 더 이상 메모리에 필요하지 않으므로 높이 맵을 릴리즈 할 수 있습니다.
	ShutdownHeightMap();

	// 지형 모델에 대한 탄젠트 및 바이 노멀을 계산합니다.
	CalculateTerrainVectors();

	// 렌더링 버퍼를 지형 데이터로 로드합니다.
	result = LoadTerrainCells();
	if (!result)
	{
		return false;
	}

	// 렌더링 버퍼가 로드된 지형 모델을 놓습니다.
	ShutdownTerrainModel();

	return true;

	return true;
}

Terrain::~Terrain()
{
	SafeDeleteArray(vertices);
	SafeDelete(vertexBuffer);

	SafeDeleteArray(indices);
	SafeDelete(indexBuffer);

	delete m_heightMap;
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

	auto verindex = GetVertexIndex(1, 1);
	string str2 = "vertexIndex : ";
	Gui::Get()->RenderText(5, 70, 1, 1, 1, str2);
	

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

	D3D::GetDC()->PSSetSamplers(10, 1, &m_sampleState);

	VPSet(viewer);
	WorldSet();

	LightSet();

}

void Terrain::Render(Camera * viewer)
{
	if (vertexBuffer != NULL && indexBuffer != NULL)
	{
		vertexBuffer->Binding();
		rasterizerState->SetState();
	}

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

	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			UINT index[4];
			index[0] = width * z + x;
			index[1] = width * (z + 1) + x;
			index[2] = width * z + (x + 1);
			index[3] = width * (z + 1) + (x + 1);

			D3DXVECTOR3 p[4];
			for (int i = 0; i < 4; i++)
			{
				p[i].x = vertices[index[i]].Position.x;
				p[i].y = vertices[index[i]].Position.y;
				p[i].z = vertices[index[i]].Position.z;

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

			if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
			{
				D3DXVECTOR3 temp = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
				Vector3 result;
				result.x = temp.x;
				result.y = temp.y;
				result.z = temp.z;
				return result;
			}
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

vector<UINT> Terrain::GetVertexIndex(float posX, float posZ)
{
	vector<UINT> vector;

	UINT index = height * posZ + posX;

	for (int index = 0; index < vertexCount; index++)
	{
		if (vertices[index].Position.x == posX && vertices[index].Position.z == posZ)
			vector.push_back(index);
	}
	//1,0
	//129

	//leftUp
	if (posZ >= 1 && posZ <= height - 1)
	{
		UINT leftUp = (6*height) * (posZ)+ (posX) + 4;//5
		vector.push_back(leftUp);
	}
	//leftDown
	if (posZ >= 1 && posZ <= height - 1)
	{
		UINT leftUp = (6 * height) * (posZ + 1) + (posX + 1);//1
		vector.push_back(leftUp);
	}
	//rightUp
	if (posZ >= 1 && posZ <= height - 1)
	{
		UINT leftUp = (6 * height) * (posZ)+(posX)+7;
		vector.push_back(leftUp);
	}
	//rightUp
	if (posZ >= 1 && posZ <= height - 1)
	{
		UINT leftUp = (6 * height) * (posZ)+(posX)+8;
		vector.push_back(leftUp);
	}
	//leftDown
	//rightDown
	//if (posZ >= 1 && posZ <= height -1)
	//{
	//	UINT left = (6*height) * (posZ-1)+ posX;//1
	//	vector.push_back(left);
	//}

	//if (posX >= 1 && posX <= width -1)
	//{
	//	UINT right = (6* height) * (posZ - 1) + posX;
	//	vector.push_back(right);
	//}

	//if (posZ < height - 1)
	//{
	//	UINT Up = (6 * height) * (posZ) + posX;
	//	vector.push_back(Up);
	//}

	//if ()
	//{
	//	UINT Down = (6 * height) * (posZ-2)+posX;
	//	vector.push_back(rightDown);
	//}
	return vector;
}

void Terrain::CopyVertexArray(void * vertexList)
{
	memcpy(vertexList, vertices, sizeof(VertexTextureNormal) * vertexCount);
}

void Terrain::CopyIndexArray(void* indexList)
{
	memcpy(indexList, indices, sizeof(UINT) * indexCount);
}



bool Terrain::LoadSetupFile(const char* filename)
{
	// 지형 파일 이름과 색상 맵 파일 이름을 포함할 문자열을 초기화합니다.
	int stringLength = 256;

	m_terrainFilename = new char[stringLength];
	if (!m_terrainFilename)
	{
		return false;
	}

	m_colorMapFilename = new char[stringLength];
	if (!m_colorMapFilename)
	{
		return false;
	}

	//// 설치 파일을 엽니다. 파일을 열 수 없으면 종료합니다.
	//ifstream fin;
	//fin.open(filename);
	//if (fin.fail())
	//{
	//	return false;
	//}

	//// 지형 파일 이름까지 읽습니다.
	//char input = 0;
	//fin.get(input);
	//while (input != ':')
	//{
	//	fin.get(input);
	//}

	//// 지형 파일 이름을 읽습니다.
	//fin >> m_terrainFilename;

	//// 지형 높이 값을 읽습니다.
	//fin.get(input);
	//while (input != ':')
	//{
	//	fin.get(input);
	//}

	//// 지형 높이를 읽습니다.
	//fin >> m_terrainHeight;

	//// 지형 너비 값을 읽습니다.
	//fin.get(input);
	//while (input != ':')
	//{
	//	fin.get(input);
	//}

	//// 지형 폭을 읽습니다.
	//fin >> m_terrainWidth;

	//// 지형 높이 배율 값을 읽습니다.
	//fin.get(input);
	//while (input != ':')
	//{
	//	fin.get(input);
	//}

	//// 지형 높이 스케일링을 읽습니다.
	//fin >> m_heightScale;

	//// 컬러 맵 파일 이름을 읽습니다.
	//fin.get(input);
	//while (input != ':')
	//{
	//	fin.get(input);
	//}

	//// 컬러 맵 파일 이름을 읽습니다.
	//fin >> m_colorMapFilename;

	//// 설정 파일을 닫습니다.
	//fin.close();

	return true;
}


bool Terrain::LoadRawHeightMap()
{
	// 높이 맵 데이터를 보관할 플로트 배열을 생성합니다.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		return false;
	}

	// 바이너리로 읽을 수 있도록 16 비트 원시 높이 맵 파일을 엽니다.
	FILE* filePtr = nullptr;
	if (fopen_s(&filePtr, m_terrainFilename, "rb") != 0)
	{
		return false;
	}

	// 원시 이미지 데이터의 크기를 계산합니다.
	int imageSize = m_terrainHeight * m_terrainWidth;

	// 원시 이미지 데이터에 메모리를 할당합니다.
	unsigned short* rawImage = new unsigned short[imageSize];
	if (!rawImage)
	{
		return false;
	}

	// 원시 이미지 데이터를 읽습니다.
	if (fread(rawImage, sizeof(unsigned short), imageSize, filePtr) != imageSize)
	{
		return false;
	}

	// 파일을 닫습니다.
	if (fclose(filePtr) != 0)
	{
		return false;
	}

	// 이미지 데이터를 높이 맵 배열에 복사합니다.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			int index = (m_terrainWidth * j) + i;

			// 높이 맵 배열에이 지점의 높이를 저장합니다.
			m_heightMap[index].y = (float)rawImage[index];
		}
	}

	// 비트 맵 이미지 데이터를 해제합니다.
	delete[] rawImage;
	rawImage = 0;

	// 이제 읽은 지형 파일 이름을 해제합니다.
	delete[] m_terrainFilename;
	m_terrainFilename = 0;

	return true;
}


void Terrain::ShutdownHeightMap()
{
	// 높이 맵 배열을 해제합니다.
	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}
}


void Terrain::SetTerrainCoordinates()
{
	// 높이 맵 배열의 모든 요소를 ​​반복하고 좌표를 올바르게 조정합니다.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			int index = (m_terrainWidth * j) + i;

			// X 및 Z 좌표를 설정합니다.
			m_heightMap[index].x = (float)i;
			m_heightMap[index].z = -(float)j;

			// 지형 깊이를 양의 범위로 이동합니다. 예를 들어 (0, -256)에서 (256, 0)까지입니다.
			m_heightMap[index].z += (float)(m_terrainHeight - 1);

			// 높이를 조절합니다.
			m_heightMap[index].y /= m_heightScale;
		}
	}
}


bool Terrain::CalculateNormals()
{
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index = 0;
	int count = 0;
	float vertex1[3] = { 0.f, 0.f, 0.f };
	float vertex2[3] = { 0.f, 0.f, 0.f };
	float vertex3[3] = { 0.f, 0.f, 0.f };
	float vector1[3] = { 0.f, 0.f, 0.f };
	float vector2[3] = { 0.f, 0.f, 0.f };
	float sum[3] = { 0.f, 0.f, 0.f };
	float length = 0.0f;


	// 정규화되지 않은 법선 벡터를 저장할 임시 배열을 생성합니다.
	VectorType* normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// 메쉬의 모든면을 살펴보고 법선을 계산합니다.
	for (int j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (int i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = ((j + 1) * m_terrainWidth) + i;      // 왼쪽 아래 꼭지점.
			index2 = ((j + 1) * m_terrainWidth) + (i + 1);  // 오른쪽 하단 정점.
			index3 = (j * m_terrainWidth) + i;          // 좌상단의 정점.

			// 표면에서 세 개의 꼭지점을 가져옵니다.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// 표면의 두 벡터를 계산합니다.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainWidth - 1)) + i;

			// 이 두 법선에 대한 정규화되지 않은 값을 얻기 위해 두 벡터의 외적을 계산합니다.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// 길이를 계산합니다.
			length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) +
				(normals[index].z * normals[index].z));

			// 길이를 사용하여이면의 최종 값을 표준화합니다.
			normals[index].x = (normals[index].x / length);
			normals[index].y = (normals[index].y / length);
			normals[index].z = (normals[index].z / length);
		}
	}

	// 이제 모든 정점을 살펴보고 각면의 평균을 취합니다. 	
	// 정점이 닿아 그 정점에 대한 평균 평균값을 얻는다.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			// 합계를 초기화합니다.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// 왼쪽 아래면.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// 오른쪽 아래 면.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// 왼쪽 위 면.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// 오른쪽 위 면.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// 이 법선의 길이를 계산합니다.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// 높이 맵 배열의 정점 위치에 대한 인덱스를 가져옵니다.
			index = (j * m_terrainWidth) + i;

			// 이 정점의 최종 공유 법선을 표준화하여 높이 맵 배열에 저장합니다.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// 임시 법선을 해제합니다.
	delete[] normals;
	normals = 0;

	return true;
}


bool Terrain::LoadColorMap()
{
	//// 바이너리로 컬러 맵 파일을 엽니다.
	//FILE* filePtr = nullptr;
	//if (fopen_s(&filePtr, m_colorMapFilename, "rb") != 0)
	//{
	//	return false;
	//}

	//// 파일 헤더를 읽습니다.
	//BITMAPFILEHEADER bitmapFileHeader;
	//if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)
	//{
	//	return false;
	//}

	//// 비트 맵 정보 헤더를 읽습니다.
	//BITMAPINFOHEADER bitmapInfoHeader;
	//if (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1)
	//{
	//	return false;
	//}

	//// 컬러 맵 치수가 쉬운 1 : 1 매핑을위한 지형 치수와 동일한 지 확인하십시오.
	//if ((bitmapInfoHeader.biWidth != m_terrainWidth) || (bitmapInfoHeader.biHeight != m_terrainHeight))
	//{
	//	return false;
	//}

	//// 비트 맵 이미지 데이터의 크기를 계산합니다.
	//// 이것은 2 차원으로 나눌 수 없으므로 (예 : 257x257) 각 행에 여분의 바이트를 추가해야합니다.
	//int imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

	//// 비트 맵 이미지 데이터에 메모리를 할당합니다.
	//unsigned char* bitmapImage = new unsigned char[imageSize];
	//if (!bitmapImage)
	//{
	//	return false;
	//}

	//// 비트 맵 데이터의 시작 부분으로 이동합니다.
	//fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//// 비트 맵 이미지 데이터를 읽습니다.
	//if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize)
	//{
	//	return false;
	//}

	//// 파일을 닫습니다.
	//if (fclose(filePtr) != 0)
	//{
	//	return false;
	//}

	//// 이미지 데이터 버퍼의 위치를 ​​초기화합니다.
	//int k = 0;

	//// 이미지 데이터를 높이 맵 구조의 색상 맵 부분으로 읽습니다.
	//for (int j = 0; j < m_terrainHeight; j++)
	//{
	//	for (int i = 0; i < m_terrainWidth; i++)
	//	{
	//		// 비트 맵은 거꾸로되어 배열의 맨 아래부터 맨 위로 로드됩니다.
	//		int index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

	//		m_heightMap[index].b = (float)bitmapImage[k] / 255.0f;
	//		m_heightMap[index].g = (float)bitmapImage[k + 1] / 255.0f;
	//		m_heightMap[index].r = (float)bitmapImage[k + 2] / 255.0f;

	//		k += 3;
	//	}

	//	// 2 비트 씩 비 - 나누기 (예 : 257x257)의 각 줄 끝에있는 여분의 바이트를 보상합니다.
	//	k++;
	//}

	//// 비트 맵 이미지 데이터를 해제합니다.
	//delete[] bitmapImage;
	//bitmapImage = 0;

	//// 이제 읽은 색상 맵 파일 이름을 해제합니다.
	//delete[] m_colorMapFilename;
	//m_colorMapFilename = 0;

	return true;
}


bool Terrain::BuildTerrainModel()
{
	// 3D 지형 모델에서 정점 수를 계산합니다.
	vertexCount = (m_terrainHeight - 1) * (m_terrainWidth - 1) * 6;

	// 3D 지형 모델 배열을 생성합니다.
	m_terrainModel = new ModelType[vertexCount];
	if (!m_terrainModel)
	{
		return false;
	}

	// 높이 맵 지형 데이터로 지형 모델을 로드합니다.
	int index = 0;

	for (int j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (int i = 0; i < (m_terrainWidth - 1); i++)
		{
			int index1 = (m_terrainWidth * j) + i;          // 왼쪽 아래.
			int index2 = (m_terrainWidth * j) + (i + 1);      // 오른쪽 아래.
			int index3 = (m_terrainWidth * (j + 1)) + i;      // 왼쪽 위.
			int index4 = (m_terrainWidth * (j + 1)) + (i + 1);  // 오른쪽 위.

			// 이제 해당 쿼드에 대해 두 개의 삼각형을 생성합니다.
			// 삼각형 1 - 왼쪽 위.
			m_terrainModel[index].x = m_heightMap[index1].x;
			m_terrainModel[index].y = m_heightMap[index1].y;
			m_terrainModel[index].z = m_heightMap[index1].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index1].nx;
			m_terrainModel[index].ny = m_heightMap[index1].ny;
			m_terrainModel[index].nz = m_heightMap[index1].nz;
			m_terrainModel[index].r = m_heightMap[index1].r;
			m_terrainModel[index].g = m_heightMap[index1].g;
			m_terrainModel[index].b = m_heightMap[index1].b;
			index++;

			// 삼각형 1 - 오른쪽 위.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			m_terrainModel[index].r = m_heightMap[index2].r;
			m_terrainModel[index].g = m_heightMap[index2].g;
			m_terrainModel[index].b = m_heightMap[index2].b;
			index++;

			// 삼각형 1 - 왼쪽 맨 아래.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			m_terrainModel[index].r = m_heightMap[index3].r;
			m_terrainModel[index].g = m_heightMap[index3].g;
			m_terrainModel[index].b = m_heightMap[index3].b;
			index++;

			// 삼각형 2 - 왼쪽 아래.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			m_terrainModel[index].r = m_heightMap[index3].r;
			m_terrainModel[index].g = m_heightMap[index3].g;
			m_terrainModel[index].b = m_heightMap[index3].b;
			index++;

			// 삼각형 2 - 오른쪽 위.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			m_terrainModel[index].r = m_heightMap[index2].r;
			m_terrainModel[index].g = m_heightMap[index2].g;
			m_terrainModel[index].b = m_heightMap[index2].b;
			index++;

			// 삼각형 2 - 오른쪽 하단.
			m_terrainModel[index].x = m_heightMap[index4].x;
			m_terrainModel[index].y = m_heightMap[index4].y;
			m_terrainModel[index].z = m_heightMap[index4].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index4].nx;
			m_terrainModel[index].ny = m_heightMap[index4].ny;
			m_terrainModel[index].nz = m_heightMap[index4].nz;
			m_terrainModel[index].r = m_heightMap[index4].r;
			m_terrainModel[index].g = m_heightMap[index4].g;
			m_terrainModel[index].b = m_heightMap[index4].b;
			index++;
		}
	}

	return true;
}


void Terrain::ShutdownTerrainModel()
{
	// 지형 모델 데이터를 공개합니다.
	if (m_terrainModel)
	{
		delete[] m_terrainModel;
		m_terrainModel = 0;
	}
}


void Terrain::CalculateTerrainVectors()
{
	//TempVertexType vertex1, vertex2, vertex3;
	//VectorType tangent, binormal;


	//// 지형 모델에서면의 수를 계산합니다.
	//int faceCount = m_vertexCount / 3;

	//// 모델 데이터에 대한 인덱스를 초기화합니다.
	//int index = 0;

	//// 모든면을 살펴보고 접선, 비공식 및 법선 벡터를 계산합니다.
	//for (int i = 0; i < faceCount; i++)
	//{
	//	// 지형 모델에서이면에 대한 세 개의 정점을 가져옵니다.
	//	vertex1.x = m_terrainModel[index].x;
	//	vertex1.y = m_terrainModel[index].y;
	//	vertex1.z = m_terrainModel[index].z;
	//	vertex1.tu = m_terrainModel[index].tu;
	//	vertex1.tv = m_terrainModel[index].tv;
	//	vertex1.nx = m_terrainModel[index].nx;
	//	vertex1.ny = m_terrainModel[index].ny;
	//	vertex1.nz = m_terrainModel[index].nz;
	//	index++;

	//	vertex2.x = m_terrainModel[index].x;
	//	vertex2.y = m_terrainModel[index].y;
	//	vertex2.z = m_terrainModel[index].z;
	//	vertex2.tu = m_terrainModel[index].tu;
	//	vertex2.tv = m_terrainModel[index].tv;
	//	vertex2.nx = m_terrainModel[index].nx;
	//	vertex2.ny = m_terrainModel[index].ny;
	//	vertex2.nz = m_terrainModel[index].nz;
	//	index++;

	//	vertex3.x = m_terrainModel[index].x;
	//	vertex3.y = m_terrainModel[index].y;
	//	vertex3.z = m_terrainModel[index].z;
	//	vertex3.tu = m_terrainModel[index].tu;
	//	vertex3.tv = m_terrainModel[index].tv;
	//	vertex3.nx = m_terrainModel[index].nx;
	//	vertex3.ny = m_terrainModel[index].ny;
	//	vertex3.nz = m_terrainModel[index].nz;
	//	index++;

	//	// 그 얼굴의 탄젠트와 바이 노멀을 계산합니다.
	//	CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

	//	// 이면에 대한 접선과 binormal을 모델 구조에 다시 저장하십시오.
	//	m_terrainModel[index - 1].tx = tangent.x;
	//	m_terrainModel[index - 1].ty = tangent.y;
	//	m_terrainModel[index - 1].tz = tangent.z;
	//	m_terrainModel[index - 1].bx = binormal.x;
	//	m_terrainModel[index - 1].by = binormal.y;
	//	m_terrainModel[index - 1].bz = binormal.z;

	//	m_terrainModel[index - 2].tx = tangent.x;
	//	m_terrainModel[index - 2].ty = tangent.y;
	//	m_terrainModel[index - 2].tz = tangent.z;
	//	m_terrainModel[index - 2].bx = binormal.x;
	//	m_terrainModel[index - 2].by = binormal.y;
	//	m_terrainModel[index - 2].bz = binormal.z;

	//	m_terrainModel[index - 3].tx = tangent.x;
	//	m_terrainModel[index - 3].ty = tangent.y;
	//	m_terrainModel[index - 3].tz = tangent.z;
	//	m_terrainModel[index - 3].bx = binormal.x;
	//	m_terrainModel[index - 3].by = binormal.y;
	//	m_terrainModel[index - 3].bz = binormal.z;
	//}
}


void Terrain::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal)
{
	float vector1[3] = { 0.0f, 0.0f, 0.0f };
	float vector2[3] = { 0.0f, 0.0f, 0.0f };
	float tuVector[2] = { 0.0f, 0.0f };
	float tvVector[2] = { 0.0f, 0.0f };


	// 이면의 두 벡터를 계산합니다.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// tu 및 tv 텍스처 공간 벡터를 계산합니다.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// 탄젠트 / 바이 노멀 방정식의 분모를 계산합니다.
	float den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// 교차 곱을 계산하고 계수로 곱하여 접선과 비 구식을 얻습니다.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// 이 법선의 길이를 계산합니다.
	float length = (float)sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// 법선을 표준화 한 다음 저장합니다.
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// 이 법선의 길이를 계산합니다.
	length = (float)sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// 법선을 표준화 한 다음 저장합니다.
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;
}


bool Terrain::LoadTerrainCells()
{
	// 각 지형 셀의 높이와 너비를 고정 33x33 꼭지점 배열로 설정합니다.
	int cellHeight = 33;
	int cellWidth = 33;

	// 지형 데이터를 저장하는데 필요한 셀 수를 계산합니다.
	int cellRowCount = (m_terrainWidth - 1) / (cellWidth - 1);
	m_cellCount = cellRowCount * cellRowCount;

	// 지형 셀 배열을 생성합니다.
	m_TerrainCells = new TerrainCellClass[m_cellCount];
	if (!m_TerrainCells)
	{
		return false;
	}

	// 모든 지형 셀을 반복하고 초기화합니다.
	for (int j = 0; j < cellRowCount; j++)
	{
		for (int i = 0; i < cellRowCount; i++)
		{
			int index = (cellRowCount * j) + i;

			if (!m_TerrainCells[index].Initialize(m_terrainModel, i, j, cellHeight, cellWidth, m_terrainWidth))
			{
				return false;
			}
		}
	}

	return true;
}


void Terrain::ShutdownTerrainCells()
{
	// 지형 셀 배열을 해제합니다.
	if (m_TerrainCells)
	{
		for (int i = 0; i < m_cellCount; i++)
		{
			m_TerrainCells[i].Shutdown();
		}

		delete[] m_TerrainCells;
		m_TerrainCells = 0;
	}
}


bool Terrain::RenderCell(int cellId, Frustum* Frustum)
{
	float maxWidth = 0.0f;
	float maxHeight = 0.0f;
	float maxDepth = 0.0f;
	float minWidth = 0.0f;
	float minHeight = 0.0f;
	float minDepth = 0.0f;

	// 지형 셀의 크기를 가져옵니다.
	m_TerrainCells[cellId].GetCellDimensions(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);

	// 셀이 표시되는지 확인합니다. 표시되지 않으면 반환하고 렌더링하지 않습니다.
	//if (!Frustum->CheckRectangle2(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth))
	//{
	//	// 추려진 셀의 수를 증가시킵니다.
	//	m_cellsCulled++;

	//	return false;
	//}

	// 보이는 경우 렌더링합니다.
	m_TerrainCells[cellId].Render();

	// 렌더 카운트에 셀의 다각형을 추가합니다.
	m_renderCount += (m_TerrainCells[cellId].GetVertexCount() / 3);

	// 실제로 그려진 셀의 수를 증가시킵니다.
	m_cellsDrawn++;

	return true;
}

void Terrain::RenderCellLines(int cellId)
{
	m_TerrainCells[cellId].RenderLineBuffers();
}



void Terrain::ReDrawNormal()
{
	for (UINT i = 0; i < indexCount / 3; i++)
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
	}

	for (UINT i = 0; i < vertexCount; i++)
	{
		XMVECTOR temp = XMLoadFloat3(&vertices[i].Normal);
		temp = XMVector3Normalize(temp);
		XMStoreFloat3(&vertices[i].Normal, temp);
	}

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