#pragma once

class TerrainCellClass
{
private:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float r, g, b;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
		XMFLOAT3 color;
	};

	struct VectorType
	{
		float x, y, z;
	};

	struct ColorVertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	TerrainCellClass();
	TerrainCellClass(const TerrainCellClass&);
	~TerrainCellClass();

	bool Initialize(void*, int, int, int, int, int);
	void Shutdown();
	void Render();
	void RenderLineBuffers();

	int GetVertexCount();
	int GetIndexCount();
	int GetLineBuffersIndexCount();
	void GetCellDimensions(float&, float&, float&, float&, float&, float&);

private:
	bool InitializeBuffers(int, int, int, int, int, ModelType*);
	void ShutdownBuffers();
	void RenderBuffers();
	void CalculateCellDimensions();
	bool BuildLineBuffers();
	void ShutdownLineBuffers();

public:
	VectorType* m_vertexList;

private:
	int m_vertexCount = 0;
	int m_indexCount = 0;
	int m_lineIndexCount = 0;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	ID3D11Buffer* m_lineVertexBuffer = nullptr;
	ID3D11Buffer* m_lineIndexBuffer = nullptr;
	float m_maxWidth = 0.0f;
	float m_maxHeight = 0.0f;
	float m_maxDepth = 0.0f;
	float m_minWidth = 0.0f;
	float m_minHeight = 0.0f;
	float m_minDepth = 0.0f;
	XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
};