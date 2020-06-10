#pragma once

class TerrainLod : public Renderer 
{
public:
	struct InitialDesc
	{
		wstring HeightMap;
		float CellSpacing;
		UINT CellsPerPatch;
		float HeightRatio;
	};
public:
	TerrainLod(Shader* shader, InitialDesc init);
	~TerrainLod();

	void Update() override;
	void Render() override;

	void BaseMap(wstring file);
	void LayerMap(wstring layer, wstring alpha);
	void NormalMap(wstring file);
private:
	bool InBounds(UINT x, UINT z);
	void CalcPatchBounds(UINT x, UINT z);
	void CalcBoundY();

	void CreateVertexData();
	void CreateIndexData();
private:
	UINT faceCount;
	UINT vertexPerPatchX;
	UINT vertexPerPatchZ;
private:
	struct VertexTerrain
	{
		Vector3 Position;
		Vector2 Uv;
		Vector2 BoundY;
	};
private:
	struct BufferDesc
	{
		float MinDistance = 1.0f;
		float MaxDistance = 500.0f;
		float MinTessellation = 1.0f;
		float MaxTessellation = 64.0f;

		float TexelCellSpaceU;
		float TexelCellSpaceV;
		float WorldCellSpace = 1.0f;
		float TerrainHeightRatio = 1.0f;

		Vector2 TextureScale = Vector2(1,1);
		float Padding[2];

		Plane WorldFrustumPlane[6];
	}bufferDesc;
private:
	Shader * shader;
	InitialDesc initDesc;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	vector<Color> heightMapPixel;
	ID3DX11EffectShaderResourceVariable* sHeightMap;
	ID3DX11EffectShaderResourceVariable* sBaseMap;
	ID3DX11EffectShaderResourceVariable* sLayerMap;
	ID3DX11EffectShaderResourceVariable* sAlphaMap;
	ID3DX11EffectShaderResourceVariable* sNormalMap;
private:
	UINT width;
	UINT height;
	VertexTerrain* vertices;
	UINT* indices;

	class Frustum* frustum;
	Camera* camera;
	Perspective* perspective;

	vector<Vector2> bounds;

	Texture* heightMap = NULL;
	Texture* baseMap = NULL;
	Texture* layerMap = NULL;
	Texture* alphaMap = NULL;
	Texture* normalMap = NULL;
	
};