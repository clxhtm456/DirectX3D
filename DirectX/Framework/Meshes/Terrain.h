#pragma once

struct HeightMapType
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
};

struct VectorType
{
	float x, y, z;
};

struct TerrainVertex
{
	Vector3 Position = Vector3(0,0,0);
	Vector2 Uv = Vector2(0, 0);
	//Vector2 TotalUv = Vector2(0, 0);
	Vector3 Normal = Vector3(0, 0, 0);
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
	void Render(Camera* viewer)override;

	void BaseMap(wstring file);
	void LayerMap(wstring file, wstring alpha);

	float GetHeight(Vector3& position);
	float GetHeightPick(Vector3& position);
	Vector3 GetPickedPosition();

	void SetHeight(float x, float z, float height);

	UINT GetHorizontalSize() { return width; }
	UINT GetVerticalSize() { return height; }

private:
	bool LoadHeightMap(const char* filename);
	bool CalculateNormals();
	bool InitializeBuffers();
	void CalculateTextureCoordinate();
	void ReDrawNormal();

	void RaiseHeight(vector<TerrainVertex*> vertexVector, float speed);
	void FallHeight(vector<TerrainVertex*> vertexVector, float speed);
	void NoiseHeight(vector<TerrainVertex*> vertexVector, float min, float max);
	void SmoothHeight(vector<TerrainVertex*> vertexVector, float value, float speed);
	void FlatHeight(vector<TerrainVertex*> vertexVector, float speed);
	void SlopeHeight(vector<TerrainVertex*> vertexVector);
	vector<TerrainVertex*> slopeVector;
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
		UINT Range = 1;
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

	vector<TerrainVertex*> SqureArea(Vector3 position, UINT type, UINT range);
	vector<TerrainVertex*> CircleArea(Vector3 position, UINT type, UINT range);

private:
	TerrainVertex* vertices;
	UINT* indices;
	HeightMapType* _heightMap;

	ConstantBuffer* brushBuffer;
	ConstantBuffer* lineBuffer;


private:
	UINT width;
	UINT height;
	UINT detail;

	
};