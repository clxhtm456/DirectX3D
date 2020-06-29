#pragma once

class TerrainObject
{
	virtual wstring ObjectType() = 0;
};

class BrushBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		Color Color;

		Vector3 Location;
		UINT Type;

		UINT Range;
		UINT Padding[2];
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
	}data;

	LineBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.Color = Color(1, 1, 1, 1);
		data.Visible = 0;

		data.Thickness = 0.01f;
		data.Size = 5.0f;
	}
};

class Terrain : public RenderingNode
{
public:
	static Terrain* Create(wstring heightFile);
	bool Init(wstring heightFile);
public:
	typedef VertexTextureNormal TerrainVertex;

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
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;

	TerrainVertex* vertices;
	UINT* indices;

	UINT indexCount, vertexCount;

private:
	UINT width;
	UINT height;

	
};