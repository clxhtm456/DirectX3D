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
	// ���� �𵨿� ���� �����͸� �� �������� ���� �����մϴ�.
	ModelType* terrainModel = (ModelType*)terrainModelPtr;

	// ������ ���ۿ� �� �ε����� ���� �����͸� �ε��մϴ�.
	if(!InitializeBuffers(nodeIndexX, nodeIndexY, cellHeight, cellWidth, terrainWidth, terrainModel))
	{
		return false;
	}

	// �� �̻� �ʿ����� ���� ���� �𵨿� ���� �����͸� �����ϴ�.
	terrainModel = 0;

	// �� ���� ũ�⸦ ����մϴ�.
	CalculateCellDimensions();

	// ����� ���� ���۸� �����Ͽ��� �� ������ ��� ���ڸ� �����մϴ�.
	return BuildLineBuffers();
}


void TerrainCellClass::Shutdown()
{
	// ���� ������ ���۸� �����Ѵ�.
	ShutdownLineBuffers();

	// �� ������ ���۸� �����մϴ�.
	ShutdownBuffers();
}


void TerrainCellClass::Render()
{
	// �׸��⸦ �غ��ϱ� ���� �׷��� ������ ���ο� �������� �ε��� ���۸� �����ϴ�.
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
	// �� ���� ���� ������ ���� ����մϴ�.
	m_vertexCount = cellHeight * cellWidth;
	// �ε��� ���� ����մϴ�.
	m_indexCount = (cellHeight - 1) * (cellWidth - 1) * 6;

	// ���� �迭�� �����մϴ�.
	VertexType* vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// �ε��� �迭�� �����մϴ�.
	unsigned long* indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// ���� �� �����Ϳ� ���� ���� / �ε��� �迭�� �ε����� �����մϴ�.
	int modelIndex = ((nodeIndexX * (cellWidth - 1)) + (nodeIndexY * (cellHeight - 1) * (terrainWidth - 1))) * 6;
	int index = 0;

	// ���� �迭�� �ε��� �迭�� �����ͷ� �ε��մϴ�.
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

	// ���� ���� ������ ����ü�� �����Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ���۸� �����մϴ�.
	if(FAILED(D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// ���� �ε��� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����մϴ�.
	if(FAILED(D3D::GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// �� ���� ���� ���� ������ �׼����ϴ� �� ���� ���� ���� �迭�� �����մϴ�.
	m_vertexList = new VectorType[m_vertexCount];
	if(!m_vertexList)
	{
		return false;
	}

	// �� ���� ���� ��ġ �������� ���� ���纻�� �����մϴ�.
	for(int i=0; i<m_vertexCount; i++)
	{
		m_vertexList[i].x = vertices[i].position.x;
		m_vertexList[i].y = vertices[i].position.y;
		m_vertexList[i].z = vertices[i].position.z;
	}

	// ���� ���۰� �����ǰ�ε� �� �迭�� �����Ͻʽÿ�.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void TerrainCellClass::ShutdownBuffers()
{
	// ���� �迭�� �����մϴ�.
	if(m_vertexList)
	{
		delete [] m_vertexList;
		m_vertexList = 0;
	}

	// �ε��� ���۸� �����մϴ�.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// ���ؽ� ���۸� �����մϴ�.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void TerrainCellClass::RenderBuffers()
{
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	D3D::GetDC()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	D3D::GetDC()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �� ������ ���ۿ��� �������Ǿ���ϴ� ������Ƽ�� ������ �����մϴ�.�� ��쿡�� �ﰢ���Դϴ�.
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void TerrainCellClass::CalculateCellDimensions()
{
	// ����� ũ�⸦ �ʱ�ȭ�մϴ�.
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

		// �ʺ� �ּҰ� �Ǵ� �ִ� ���� �ʰ��ϴ��� Ȯ���Ͻʽÿ�.
		if(width > m_maxWidth)
		{
			m_maxWidth = width;
		}
		if(width < m_minWidth)
		{
			m_minWidth = width;
		}

		// ���̰� �ּҰ� �Ǵ� �ִ� ���� �ʰ��ϴ��� Ȯ���մϴ�.
		if(height > m_maxHeight)
		{
			m_maxHeight = height;
		}
		if(height < m_minHeight)
		{
			m_minHeight = height;
		}

		// ���̰� �ּҰ� �Ǵ� �ִ� ���� �ʰ��ϴ��� Ȯ���մϴ�.
		if(depth > m_maxDepth)
		{
			m_maxDepth = depth;
		}
		if(depth < m_minDepth)
		{
			m_minDepth = depth;
		}
	}

	// /�� ���� ��� ��ġ�� ����մϴ�.
	m_position.x = (m_maxWidth - m_minWidth) + m_minWidth;
	m_position.y = (m_maxHeight - m_minHeight) + m_minHeight;
	m_position.z = (m_maxDepth - m_minDepth) + m_minDepth;
}


bool TerrainCellClass::BuildLineBuffers()
{
	// �� ������ ������������ �����մϴ�.
	XMFLOAT4 lineColor = XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);

	// ���� �迭�� ���� ���� �����մϴ�.
	int vertexCount = 24;

	// �ε��� �迭�� �ε��� ���� �����մϴ�.
	int indexCount = vertexCount;

	// ���� �迭�� �����մϴ�.
	ColorVertexType* vertices = new ColorVertexType[vertexCount];
	if(!vertices)
	{
		return false;
	}

	// �ε��� �迭�� �����մϴ�.
	unsigned long* indices = new unsigned long[indexCount];
	if(!indices)
	{
		return false;
	}

	// ���� ������ ����ü�� �����Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ColorVertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// �ε��� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// ������ �ε��� �迭�� �����͸� �ε��մϴ�.
	int index = 0;

	// 8 ����.
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

	// 4 ������.
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

	// ���� ���۸� �����.
	if(FAILED(D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_lineVertexBuffer)))
	{
		return false;
	}

	// �ε��� ���۸� �����մϴ�.
	if(FAILED(D3D::GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_lineIndexBuffer)))
	{
		return false;
	}

	// �������� ���� �ε��� ���� �����մϴ�.
	m_lineIndexCount = indexCount;

	// ���� ���ؽ��� �ε��� ���۰� �����ǰ� �ε�� �迭�� �����Ͻʽÿ�.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void TerrainCellClass::ShutdownLineBuffers()
{
	// �ε��� ���۸� �����մϴ�.
	if(m_lineIndexBuffer)
	{
		m_lineIndexBuffer->Release();
		m_lineIndexBuffer = 0;
	}

	// ���ؽ� ���۸� �����մϴ�.
	if(m_lineVertexBuffer)
	{
		m_lineVertexBuffer->Release();
		m_lineVertexBuffer = 0;
	}
}


void TerrainCellClass::RenderLineBuffers()
{
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(ColorVertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	D3D::GetDC()->IASetVertexBuffers(0, 1, &m_lineVertexBuffer, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	D3D::GetDC()->IASetIndexBuffer(m_lineIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �� ��� ���� ���ۿ��� �������Ǿ���ϴ� ������Ƽ�� ������ �����մϴ�.
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