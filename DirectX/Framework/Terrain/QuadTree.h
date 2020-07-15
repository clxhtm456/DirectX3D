#pragma once

const int MAX_TRIANGLES = 10000;

class QuadTree
{
private:
	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		NodeType* nodes[4];
	};
public:
	QuadTree(Terrain* terrain);
	QuadTree(const QuadTree&);
	~QuadTree();

	bool Initialize();
	void Render(Camera* viewer);

	int GetDrawCount();

private:
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(NodeType*, float, float, float);
	int CountTriangles(float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void ReleaseNode(NodeType*);
	void RenderNode(NodeType*, Camera*);

private:
	int m_triangleCount, m_drawCount;
	VertexTextureNormal* m_vertexList = nullptr;
	NodeType* m_parentNode = nullptr;
private:
	Terrain* m_Terrain;
	Shader* shader;
};