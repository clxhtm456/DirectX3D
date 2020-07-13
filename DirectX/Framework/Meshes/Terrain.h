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
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();
	void CalculateTextureCoordinate();
	void RaiseHeight(Vector3& position, UINT type, UINT range);

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

private:
	VertexTextureNormal* vertices;
	UINT* indices;
	HeightMapType* _heightMap;

	ConstantBuffer* brushBuffer;
	ConstantBuffer* lineBuffer;


private:
	UINT width;
	UINT height;
	UINT detail;

	
};