#include "Framework.h"
#include "QuadTree.h"
#include "Viewer/Frustum.h"

QuadTree::QuadTree(Terrain* terrain)
{
	m_Terrain = terrain;

	shader = m_Terrain->GetVSShader();

	Initialize();
}

QuadTree::QuadTree(const QuadTree &)
{
}

QuadTree::~QuadTree()
{
	if (m_parentNode)
	{
		ReleaseNode(m_parentNode);
		delete m_parentNode;
		m_parentNode = NULL;
	}
}

bool QuadTree::Initialize()
{
	float centerX = 0.0f;
	float centerZ = 0.0f;
	float width = 0.0f;

	// 지형 정점 배열의 정점 수를 가져옵니다.
	int vertexCount = m_Terrain->GetVertexCount();
	int indexCount = m_Terrain->GetIndexCount();

	// 정점리스트의 총 삼각형 수를 저장합니다.
	m_triangleCount = indexCount / 3;

	// 모든 지형 정점을 포함하는 정점 배열을 만듭니다.
	m_vertexList = new VertexTextureNormal[vertexCount];
	if (!m_vertexList)
	{
		return false;
	}
	m_indexList = new UINT[indexCount];
	if (!m_indexList)
	{
		return false;
	}

	// 지형 정점을 정점 목록에 복사합니다.
	m_Terrain->CopyVertexArray((void*)m_vertexList);
	m_Terrain->CopyIndexArray((void*)m_indexList);

	// 중심 x, z와 메쉬의 너비를 계산합니다.
	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

	// 쿼드 트리의 부모 노드를 만듭니다.
	m_parentNode = new NodeType;
	if (!m_parentNode)
	{
		return false;
	}

	// 정점 목록 데이터와 메쉬 차원을 기반으로 쿼드 트리를 재귀 적으로 빌드합니다.
	CreateTreeNode(m_parentNode, centerX, centerZ, width);

	// 쿼드 트리가 각 노드에 정점을 갖기 때문에 정점 목록을 놓습니다.
	if (m_vertexList)
	{
		delete[] m_vertexList;
		m_vertexList = 0;
	}

	if (m_indexList)
	{
		delete[] m_indexList;
		m_indexList = 0;
	}

	return true;
}


void QuadTree::Render(Camera* viewer)
{
	// 이 프레임에 대해 그려지는 삼각형의 수를 초기화합니다.
	m_drawCount = 0;

	// 부모 노드에서 시작하여 트리 아래로 이동하여 보이는 각 노드를 렌더링합니다.
	RenderNode(m_parentNode, viewer);

	int i = m_drawCount;
}

int QuadTree::GetDrawCount()
{
	return m_drawCount;
}




void QuadTree::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
{
	// 메쉬의 중심 위치를 0으로 초기화합니다.
	centerX = 0.0f;
	centerZ = 0.0f;

	// 메쉬의 모든 정점을 합친다.
	for (int i = 0; i < vertexCount; i++)
	{
		centerX += m_vertexList[i].Position.x;
		centerZ += m_vertexList[i].Position.z;
	}

	// 그리고 메쉬의 중간 점을 찾기 위해 정점의 수로 나눕니다.
	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	// 메쉬의 최대 및 최소 크기를 초기화합니다.
	float maxWidth = 0.0f;
	float maxDepth = 0.0f;

	float minWidth = fabsf(m_vertexList[0].Position.x - centerX);
	float minDepth = fabsf(m_vertexList[0].Position.z - centerZ);

	// 모든 정점을 살펴보고 메쉬의 최대 너비와 최소 너비와 깊이를 찾습니다.
	for (int i = 0; i < vertexCount; i++)
	{
		float width = fabsf(m_vertexList[i].Position.x - centerX);
		float depth = fabsf(m_vertexList[i].Position.z - centerZ);

		if (width > maxWidth) { maxWidth = width; }
		if (depth > maxDepth) { maxDepth = depth; }
		if (width < minWidth) { minWidth = width; }
		if (depth < minDepth) { minDepth = depth; }
	}

	// 최소와 최대 깊이와 너비 사이의 절대 최대 값을 찾습니다.
	float maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	float maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

	// 메쉬의 최대 직경을 계산합니다.
	meshWidth = max(maxX, maxZ) * 2.0f;
}

void QuadTree::CreateTreeNode(NodeType* node, float positionX, float positionZ, float width)
{
	// 노드의 위치와 크기를 저장한다.
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->width = width;

	// 노드의 삼각형 수를 0으로 초기화합니다.
	node->triangleCount = 0;

	//정점 및 인덱스 버퍼를 null로 초기화합니다.
	node->indexBuffer = 0;


	// 이 노드의 자식 노드를 null로 초기화합니다.
	node->nodes[0] = 0;
	node->nodes[1] = 0;
	node->nodes[2] = 0;
	node->nodes[3] = 0;

	// 이 노드 안에 있는 삼각형 수를 센다.
	int numTriangles = CountTriangles(positionX, positionZ, width);

	// 사례 1: 이 노드에 삼각형이 없으면 비어있는 상태로 돌아가서 처리할 필요가 없습니다.
	if (numTriangles == 0)
	{
		return;
	}

	// 사례 2: 이 노드에 너무 많은 삼각형이 있는 경우 4 개의 동일한 크기의 더 작은 트리 노드로 분할합니다.
	if (numTriangles > MAX_TRIANGLES)
	{
		for (int i = 0; i < 4; i++)
		{
			// 새로운 자식 노드에 대한 위치 오프셋을 계산합니다.
			float offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			float offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			// 새 노드에 삼각형이 있는지 확인합니다.
			int count = CountTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			if (count > 0)
			{
				// 이 새 노드가있는 삼각형이있는 경우 자식 노드를 만듭니다.
				node->nodes[i] = new NodeType;

				// 이제이 새 자식 노드에서 시작하는 트리를 확장합니다.
				CreateTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			}
		}
		return;
	}

	// 사례 3: 이 노드가 비어 있지않고 그 노드의 삼각형 수가 최대 값보다 작으면 
	// 이 노드는 트리의 맨 아래에 있으므로 저장할 삼각형 목록을 만듭니다.
	node->triangleCount = numTriangles;

	// 정점의 수를 계산합니다.
	int vertexCount = numTriangles * 3;

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[vertexCount];


	// 이 새로운 정점 및 인덱스 배열의 인덱스를 초기화합니다.
	int index = 0;
	int IndicesIndex = 0;

	// 정점 목록의 모든 삼각형을 살펴 봅니다.
	for (int i = 0; i < m_triangleCount; i++)
	{
		// 삼각형이이 노드 안에 있으면 꼭지점 배열에 추가합니다.
		if (IsTriangleContained(i, positionX, positionZ, width) == true)
		{
			// 지형 버텍스 목록에 인덱스를 계산합니다.
			IndicesIndex = i * 3;

			// 정점 목록에서 이 삼각형의 세 꼭지점을 가져옵니다.
			indices[index] = m_indexList[IndicesIndex];
			index++;

			IndicesIndex++;
			indices[index] = m_indexList[IndicesIndex];
			index++;

			IndicesIndex++;
			indices[index] = m_indexList[IndicesIndex];
			index++;
		}
	}

	// 인덱스 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * vertexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 만듭니다.
	D3D::GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &node->indexBuffer);

	delete[] indices;
	indices = 0;
}

int QuadTree::CountTriangles(float positionX, float positionZ, float width)
{
	// 카운트를 0으로 초기화한다.
	int count = 0;

	// 전체 메쉬의 모든 삼각형을 살펴보고 어떤 노드가 이 노드 안에 있어야 하는지 확인합니다.
	for (int i = 0; i < m_triangleCount; i++)
	{
		// 삼각형이 노드 안에 있으면 1씩 증가시킵니다.
		if (IsTriangleContained(i, positionX, positionZ, width) == true)
		{
			count++;
		}
	}

	return count;
}

bool QuadTree::IsTriangleContained(int index, float positionX, float positionZ, float width)
{
	// 이 노드의 반경을 계산합니다.
	float radius = width / 2.0f;

	// 인덱스를 정점 목록으로 가져옵니다.
	int IndicesIndex = index * 3;
	int vertexIndex = m_indexList[IndicesIndex];

	// 정점 목록에서 이 삼각형의 세 꼭지점을 가져옵니다.
	float x1 = m_vertexList[vertexIndex].Position.x;
	float z1 = m_vertexList[vertexIndex].Position.z;
	IndicesIndex++;
	vertexIndex = m_indexList[IndicesIndex];

	float x2 = m_vertexList[vertexIndex].Position.x;
	float z2 = m_vertexList[vertexIndex].Position.z;
	IndicesIndex++;
	vertexIndex = m_indexList[IndicesIndex];

	float x3 = m_vertexList[vertexIndex].Position.x;
	float z3 = m_vertexList[vertexIndex].Position.z;

	// 삼각형의 x 좌표의 최소값이 노드 안에 있는지 확인하십시오.
	float minimumX = min(x1, min(x2, x3));
	if (minimumX > (positionX + radius))
	{
		return false;
	}

	// 삼각형의 x 좌표의 최대 값이 노드 안에 있는지 확인하십시오.
	float maximumX = max(x1, max(x2, x3));
	if (maximumX < (positionX - radius))
	{
		return false;
	}

	// 삼각형의 z 좌표의 최소값이 노드 안에 있는지 확인하십시오.
	float minimumZ = min(z1, min(z2, z3));
	if (minimumZ > (positionZ + radius))
	{
		return false;
	}

	// 삼각형의 z 좌표의 최대 값이 노드 안에 있는지 확인하십시오.
	float maximumZ = max(z1, max(z2, z3));
	if (maximumZ < (positionZ - radius))
	{
		return false;
	}

	return true;
}

void QuadTree::ReleaseNode(NodeType * node)
{
	// 재귀 적으로 트리 아래로 내려와 맨 아래 노드를 먼저 놓습니다.
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			ReleaseNode(node->nodes[i]);
		}
	}


	// 이 노드의 인덱스 버퍼를 해제합니다.
	if (node->indexBuffer)
	{
		node->indexBuffer->Release();
		node->indexBuffer = 0;
	}


	// 네 개의 자식 노드를 해제합니다.
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i])
		{
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}
}

void QuadTree::RenderNode(NodeType* node, Camera* viewer)
{
	// 노드를 볼 수 있는지, 높이는 쿼드 트리에서 중요하지 않은지 확인합니다.
	// 보이지 않는 경우 자식 중 하나도 트리 아래로 계속 진행할 수 없으며 속도가 증가한 곳입니다.
	if (!viewer->GetFrustum()->CheckCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f)))
	{
		return;
	}

	// 볼 수 있는 경우 네 개의 자식 노드를 모두 확인하여 볼 ​​수 있는지 확인합니다.
	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			count++;
			RenderNode(node->nodes[i], viewer);
		}
	}

	// 자식 노드가 있는 경우 부모 노드가 렌더링 할 삼각형을 포함하지 않으므로 계속할 필요가 없습니다.
	if (count != 0)
	{
		return;
	}

	// 그렇지 않으면 이 노드를 볼 수 있고 그 안에 삼각형이 있으면 이 삼각형을 렌더링합니다.

	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexTextureNormal);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	D3D::GetDC()->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 꼭지점 버퍼에서 렌더링 되어야 하는 프리미티브 유형을 설정합니다. 이 경우에는 삼각형입니다.
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 이 노드에서 인덱스의 수를 결정합니다.
	int indexCount = node->triangleCount * 3;

	
	shader->Binding();

	ID3D11SamplerState* sampleState = &m_Terrain->GetSampleState();
	// 픽셀 쉐이더에서 샘플러 상태를 설정합니다.
	D3D::GetDC()->PSSetSamplers(0, 1, &sampleState);

	// 삼각형을 그립니다.
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	

	// 이 프레임 동안 렌더링 된 폴리곤의 수를 늘립니다.
	m_drawCount += node->triangleCount;
}


void QuadTree::SearchVertex(NodeType* node, UINT x, UINT z)
{
	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			count++;
			SearchVertex(node->nodes[i],x,z);
		}
	}

	float margin = node->width * 0.5f;
	if (x < node->positionX - margin ||
		x > node->positionX + margin ||
		z < node->positionZ - margin ||
		z > node->positionZ + margin)
		return;

	// 자식 노드가 있는 경우 부모 노드가 렌더링 할 삼각형을 포함하지 않으므로 계속할 필요가 없습니다.
	if (count != 0)
	{
		return;
	}


	return;
}

void QuadTree::QuadVertex(UINT x, UINT z)
{
	SearchVertex(m_parentNode, x, z);

	return;
	// TODO: 여기에 반환 구문을 삽입합니다.
}
