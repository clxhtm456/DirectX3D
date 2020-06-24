#pragma once

class BrushBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		Color Color;
		Vector3 Location;
		UINT Type = 0;
		UINT Range = 1;
	}data;

	BrushBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.Color = XMCOLOR(0, 1, 0, 1);
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
		data.Color = XMCOLOR(1, 1, 1, 1);
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

	void Update();
	void Render();

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
	Shader* shader;
	Texture* heightMap;

	BrushBuffer* brushBuffer;
	LineBuffer* lineBuffer;

	TerrainVertex* vertices;
	UINT* indices;

	Texture* baseMap = NULL;

	Texture* layerMap = NULL;

	Texture* alphaMap = NULL;

private:
	UINT width;
	UINT height;

	
};