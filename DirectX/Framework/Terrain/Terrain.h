#pragma once

struct HeightMapType
{
	float x, y, z;
	float nx, ny, nz;
	float r, g, b;
};

struct ModelType
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
	float tx, ty, tz;
	float bx, by, bz;
	float r, g, b;
};

struct VectorType
{
	float x, y, z;
};

struct TerrainVertex
{
	Vector3 Position = Vector3(0,0,0);
	Vector2 Uv = Vector2(0, 0);
	Vector3 Normal = Vector3(0, 0, 0);
};

struct TempVertexType
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
};

class Terrain : public RenderingNode
{
public:
	static Terrain* Create(UINT horizontal, UINT vertical, UINT textureDetail = 10);
	bool Init(UINT horizontal, UINT vertical, UINT textureDetail);

public:
	Terrain();
	~Terrain();

	void Update()override;
	void ResourceBinding(Camera* viewer)override;
	void Render(Camera* viewer)override;

	void BaseMap(wstring file);
	void LayerMap(wstring file, wstring alpha);

	float GetHeight(Vector3& position);
	float GetHeightPick(Vector3& position);
	Vector3 GetPickedPosition();
	UINT GetVertexCount() { return vertexCount; }
	UINT GetIndexCount() { return indexCount; }

	void SetHeight(float x, float z, float height);

	UINT GetHorizontalSize() { return width; }
	UINT GetVerticalSize() { return height; }

	vector<UINT> GetVertexIndex(float posX, float posZ);

	void CopyVertexArray(void* vertexList);
	void CopyIndexArray(void* indexList);
	ID3D11SamplerState& GetSampleState() { return (*m_sampleState); }
private:
	bool LoadSetupFile(const char*);
	bool LoadRawHeightMap();
	void ShutdownHeightMap();
	void SetTerrainCoordinates();
	bool CalculateNormals();
	bool LoadColorMap();
	bool BuildTerrainModel();
	void ShutdownTerrainModel();
	void CalculateTerrainVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	bool LoadTerrainCells();
	void ShutdownTerrainCells();

	bool RenderCell(int, class Frustum*);
	void RenderCellLines(int);

	void ReDrawNormal();
	void RaiseHeight(vector<VertexTextureNormal*> vertexVector, float speed);
	void FallHeight(vector<VertexTextureNormal*> vertexVector, float speed);
	void NoiseHeight(vector<VertexTextureNormal*> vertexVector, float min, float max);
	void SmoothHeight(vector<VertexTextureNormal*> vertexVector, float value, float speed);
	void FlatHeight(vector<VertexTextureNormal*> vertexVector, float speed);
	void SlopeHeight(vector<VertexTextureNormal*> vertexVector);
	vector<VertexTextureNormal*> slopeVector;
private:
	Texture* heightMap = NULL;
	Texture* baseMap = NULL;
	Texture* layerMap = NULL;
	Texture* alphaMap = NULL;
private:
	struct BrushDesc
	{
		Vector4 Color = Vector4(0, 1, 0, 1);
		Vector3 Location;
		UINT Type = 0;
		UINT Range = 10;
		float Padding[3];
	} brushDesc;

	struct LineDesc
	{
		Vector4 Color = Vector4(1, 1, 1, 1);
		UINT Visible = 0;
		float Thickness = 0.01f;
		float Size = 5.0f;
		float Padding;
	} lineDesc;

	char* BrushName[3] = { "None","Squre","Circle" };
	bool BrushType[3] = { true,false,false };

	char* BrushStyleName[6] = { "Raise","Fall","Noise","Smooth","Flat","Slope" };
	bool BrushStyle[6] = { true,false, };
	int IBrushStyle = 0;

	vector<VertexTextureNormal*> SqureArea(Vector3 position, UINT type, UINT range);
	vector<VertexTextureNormal*> CircleArea(Vector3 position, UINT type, UINT range);

private:
	VertexTextureNormal* vertices;
	UINT* indices;

	ConstantBuffer* brushBuffer;
	ConstantBuffer* lineBuffer;


	ID3D11SamplerState* m_sampleState;
private:
	UINT width;
	UINT height;
	UINT detail;
private:
	class QuadTree* m_QuadTree;

	int m_terrainHeight = 0;
	int m_terrainWidth = 0;
	float m_heightScale = 0.0f;
	char* m_terrainFilename = nullptr;
	char* m_colorMapFilename = nullptr;

	HeightMapType* m_heightMap = nullptr;
	ModelType* m_terrainModel = nullptr;

	class TerrainCellClass* m_TerrainCells = nullptr;

	int m_cellCount = 0;
	int m_renderCount = 0;
	int m_cellsDrawn = 0;
	int m_cellsCulled = 0;

	
};