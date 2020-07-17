#include "Framework.h"
#include "terraincellclass.h"


TerrainCellClass::TerrainCellClass()
{
}


TerrainCellClass::TerrainCellClass(const TerrainCellClass& other)
{
}


TerrainCellClass::~TerrainCellClass()
{
}


bool TerrainCellClass::Initialize(void* terrainModelPtr, int nodeIndexX, int nodeIndexY, 
								  int cellHeight, int cellWidth, int terrainWidth)
{
	// 지형 모델에 대한 포인터를 모델 유형으로 강제 변형합니다.
	ModelType* terrainModel = (ModelType*)terrainModelPtr;

	// 렌더링 버퍼에 셀 인덱스의 지형 데이터를 로드합니다.
	if(!InitializeBuffers(nodeIndexX, nodeIndexY, cellHeight, cellWidth, terrainWidth, terrainModel))
	{
		return false;
	}

	// 더 이상 필요하지 않은 지형 모델에 대한 포인터를 놓습니다.
	terrainModel = 0;

	// 이 셀의 크기를 계산합니다.
	CalculateCellDimensions();

	// 디버그 라인 버퍼를 빌드하여이 셀 주위에 경계 상자를 생성합니다.
	return BuildLineBuffers();
}


void TerrainCellClass::Shutdown()
{
	// 라인 렌더링 버퍼를 해제한다.
	ShutdownLineBuffers();

	// 셀 렌더링 버퍼를 해제합니다.
	ShutdownBuffers();
}


void TerrainCellClass::Render()
{
	// 그리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓습니다.
	RenderBuffers();
}


int TerrainCellClass::GetVertexCount()
{
	return m_vertexCount;
}


int TerrainCellClass::GetIndexCount()
{
	return m_indexCount;
}


bool TerrainCellClass::InitializeBuffers(int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, 
										 int terrainWidth, ModelType* terrainModel)
{	
	// 이 지형 셀의 꼭지점 수를 계산합니다.
	m_vertexCount = cellHeight * cellWidth;
	// 인덱스 수를 계산합니다.
	m_indexCount = (cellHeight - 1) * (cellWidth - 1) * 6;

	// 정점 배열을 생성합니다.
	VertexType* vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// 인덱스 배열을 생성합니다.
	unsigned long* indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// 지형 모델 데이터와 로컬 정점 / 인덱스 배열에 인덱스를 설정합니다.
	int modelIndex = ((nodeIndexX * (cellWidth - 1)) + (nodeIndexY * (cellHeight - 1) * (terrainWidth - 1))) * 6;
	int index = 0;

	// 정점 배열과 인덱스 배열을 데이터로 로드합니다.
	for(int j=0; j<(cellHeight - 1); j++)
	{
		for(int i=0; i<((cellWidth - 1) * 6); i++)
		{
			vertices[index].position = XMFLOAT3(terrainModel[modelIndex].x, terrainModel[modelIndex].y, terrainModel[modelIndex].z);
			vertices[index].texture = XMFLOAT2(terrainModel[modelIndex].tu, terrainModel[modelIndex].tv);
			vertices[index].normal = XMFLOAT3(terrainModel[modelIndex].nx, terrainModel[modelIndex].ny, terrainModel[modelIndex].nz);
			vertices[index].tangent = XMFLOAT3(terrainModel[modelIndex].tx, terrainModel[modelIndex].ty, terrainModel[modelIndex].tz);
			vertices[index].binormal = XMFLOAT3(terrainModel[modelIndex].bx, terrainModel[modelIndex].by, terrainModel[modelIndex].bz);
			vertices[index].color = XMFLOAT3(terrainModel[modelIndex].r, terrainModel[modelIndex].g, terrainModel[modelIndex].b);
			indices[index] = index;
			modelIndex++;
			index++;
		}
		modelIndex += (terrainWidth * 6) - (cellWidth * 6);
	}

	// 정적 정점 버퍼의 구조체를 설정한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 생성합니다.
	if(FAILED(D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if(FAILED(D3D::GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// 이 셀에 대한 정점 정보에 액세스하는 데 사용될 공용 정점 배열을 생성합니다.
	m_vertexList = new VectorType[m_vertexCount];
	if(!m_vertexList)
	{
		return false;
	}

	// 이 셀의 정점 위치 데이터의 로컬 복사본을 유지합니다.
	for(int i=0; i<m_vertexCount; i++)
	{
		m_vertexList[i].x = vertices[i].position.x;
		m_vertexList[i].y = vertices[i].position.y;
		m_vertexList[i].z = vertices[i].position.z;
	}

	// 이제 버퍼가 생성되고로드 된 배열을 해제하십시오.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void TerrainCellClass::ShutdownBuffers()
{
	// 정점 배열을 해제합니다.
	if(m_vertexList)
	{
		delete [] m_vertexList;
		m_vertexList = 0;
	}

	// 인덱스 버퍼를 해제합니다.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 버텍스 버퍼를 해제합니다.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void TerrainCellClass::RenderBuffers()
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	D3D::GetDC()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	D3D::GetDC()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 꼭지점 버퍼에서 렌더링되어야하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void TerrainCellClass::CalculateCellDimensions()
{
	// 노드의 크기를 초기화합니다.
	m_maxWidth = -1000000.0f;
	m_maxHeight = -1000000.0f;
	m_maxDepth = -1000000.0f;

	m_minWidth = 1000000.0f;
	m_minHeight = 1000000.0f;
	m_minDepth = 1000000.0f;

	for(int i=0; i<m_vertexCount; i++)
	{
		float width = m_vertexList[i].x;
		float height = m_vertexList[i].y;
		float depth = m_vertexList[i].z;

		// 너비가 최소값 또는 최대 값을 초과하는지 확인하십시오.
		if(width > m_maxWidth)
		{
			m_maxWidth = width;
		}
		if(width < m_minWidth)
		{
			m_minWidth = width;
		}

		// 높이가 최소값 또는 최대 값을 초과하는지 확인합니다.
		if(height > m_maxHeight)
		{
			m_maxHeight = height;
		}
		if(height < m_minHeight)
		{
			m_minHeight = height;
		}

		// 깊이가 최소값 또는 최대 값을 초과하는지 확인합니다.
		if(depth > m_maxDepth)
		{
			m_maxDepth = depth;
		}
		if(depth < m_minDepth)
		{
			m_minDepth = depth;
		}
	}

	// /이 셀의 가운데 위치를 계산합니다.
	m_position.x = (m_maxWidth - m_minWidth) + m_minWidth;
	m_position.y = (m_maxHeight - m_minHeight) + m_minHeight;
	m_position.z = (m_maxDepth - m_minDepth) + m_minDepth;
}


bool TerrainCellClass::BuildLineBuffers()
{
	// 줄 색상을 오렌지색으로 설정합니다.
	XMFLOAT4 lineColor = XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);

	// 정점 배열의 정점 수를 설정합니다.
	int vertexCount = 24;

	// 인덱스 배열의 인덱스 수를 설정합니다.
	int indexCount = vertexCount;

	// 정점 배열을 생성합니다.
	ColorVertexType* vertices = new ColorVertexType[vertexCount];
	if(!vertices)
	{
		return false;
	}

	// 인덱스 배열을 생성합니다.
	unsigned long* indices = new unsigned long[indexCount];
	if(!indices)
	{
		return false;
	}

	// 정점 버퍼의 구조체를 설정한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ColorVertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 정점과 인덱스 배열에 데이터를 로드합니다.
	int index = 0;

	// 8 수평선.
	vertices[index].position = XMFLOAT3(m_minWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	// 4 수직선.
	vertices[index].position = XMFLOAT3(m_maxWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;

	// 정점 버퍼를 만든다.
	if(FAILED(D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_lineVertexBuffer)))
	{
		return false;
	}

	// 인덱스 버퍼를 생성합니다.
	if(FAILED(D3D::GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_lineIndexBuffer)))
	{
		return false;
	}

	// 렌더링을 위해 인덱스 수를 저장합니다.
	m_lineIndexCount = indexCount;

	// 이제 버텍스와 인덱스 버퍼가 생성되고 로드된 배열을 해제하십시오.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void TerrainCellClass::ShutdownLineBuffers()
{
	// 인덱스 버퍼를 해제합니다.
	if(m_lineIndexBuffer)
	{
		m_lineIndexBuffer->Release();
		m_lineIndexBuffer = 0;
	}

	// 버텍스 버퍼를 해제합니다.
	if(m_lineVertexBuffer)
	{
		m_lineVertexBuffer->Release();
		m_lineVertexBuffer = 0;
	}
}


void TerrainCellClass::RenderLineBuffers()
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(ColorVertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	D3D::GetDC()->IASetVertexBuffers(0, 1, &m_lineVertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	D3D::GetDC()->IASetIndexBuffer(m_lineIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 경우 정점 버퍼에서 렌더링되어야하는 프리미티브 유형을 설정합니다.
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}


int TerrainCellClass::GetLineBuffersIndexCount()
{
	return m_lineIndexCount;
}


void TerrainCellClass::GetCellDimensions(float& maxWidth, float& maxHeight, float& maxDepth, 
										 float& minWidth, float& minHeight, float& minDepth)
{
	maxWidth = m_maxWidth;
	maxHeight = m_maxHeight;
	maxDepth = m_maxDepth;
	minWidth = m_minWidth;
	minHeight = m_minHeight;
	minDepth = m_minDepth;
}