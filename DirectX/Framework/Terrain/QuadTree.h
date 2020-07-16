#pragma once

const int MAX_TRIANGLES = 10000;

class QuadTree
{
private:
	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer *indexBuffer;
		NodeType* nodes[4];
	};
public:
	QuadTree(Terrain* terrain);
	QuadTree(const QuadTree&);
	~QuadTree();

	bool Initialize();
	void Render(Camera* viewer);

	int GetDrawCount();
	void QuadVertex(UINT x, UINT z);
private:
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(NodeType*, float, float, float);
	int CountTriangles(float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void ReleaseNode(NodeType*);
	void RenderNode(NodeType*, Camera*);

	void SearchVertex(NodeType* node, UINT x, UINT z);
private:
	int m_triangleCount, m_drawCount;
	VertexTextureNormal* m_vertexList = nullptr;
	UINT* m_indexList = nullptr;
	NodeType* m_parentNode = nullptr;
private:
	Terrain* m_Terrain;
	Shader* shader;
};