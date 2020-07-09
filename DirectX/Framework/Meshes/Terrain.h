#pragma once

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
	VertexTextureNormal* vertices;
	UINT* indices;

private:
	UINT width;
	UINT height;
	UINT detail;

	
};