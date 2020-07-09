#pragma once

class BrushBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		Color Color;

		Vector3 Location;
		UINT Type;

		UINT Range;
		UINT Padding[3];
	}data;

	BrushBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.Color = Color(0, 1, 0, 1);
		data.Location = Vector3(0, 0, 0);

		data.Type = 0;
		data.Range = 1;
	}
};

class LineBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		Color Color;
		UINT Visible;
		float Thickness;
		float Size;
		float Padding;
	}data;

	LineBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.Color = Color(1, 1, 1, 1);
		data.Visible = 0;

		data.Thickness = 0.01f;
		data.Size = 5.0f;
	}
};

class TerrainEdit : public RenderingNode
{
public:
	static TerrainEdit* Create(UINT horizontal, UINT vertical, UINT textureDetail = 10);
	bool Init(UINT horizontal, UINT vertical, UINT textureDetail);

public:
	TerrainEdit();
	~TerrainEdit();

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
	void RaiseHeight(Vector3& position, UINT type, UINT range);

private:
	Texture* heightMap = NULL;
	Texture* baseMap = NULL;
	Texture* layerMap = NULL;
	Texture* alphaMap = NULL;

	BrushBuffer* brushBuffer;
	LineBuffer* lineBuffer;
private:
	VertexTextureNormal* vertices;
	UINT* indices;

private:
	UINT width;
	UINT height;
	UINT detail;

	
};