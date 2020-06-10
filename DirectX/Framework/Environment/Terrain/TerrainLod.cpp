#include "Framework.h"
#include "TerrainLod.h"
#include "Viewer/Fixity.h"
#include "Objects/Frustum.h"

TerrainLod::TerrainLod(Shader * shader, InitialDesc init):
	Renderer(shader), 
	initDesc(init)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	sBaseMap = shader->AsSRV("BaseMap");
	sAlphaMap = shader->AsSRV("AlphaMap");
	sLayerMap = shader->AsSRV("LayerMap");
	sNormalMap = shader->AsSRV("NormalMap");
	sHeightMap = shader->AsSRV("HeightMap");

	buffer = new ConstantBuffer(&bufferDesc, sizeof(BufferDesc));
	sBuffer = shader->AsConstantBuffer("CB_TerrainLod");

	heightMap = new Texture(initDesc.HeightMap);
	sHeightMap->SetResource(heightMap->SRV());
	heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UINT, &heightMapPixel);

	width = heightMap->GetWidth() -1;
	height = heightMap->GetHeight() - 1;

	vertexPerPatchX = (width / initDesc.CellsPerPatch) + 1;
	vertexPerPatchZ = (height / initDesc.CellsPerPatch) + 1;

	vertexCount = vertexPerPatchX * vertexPerPatchZ;
	faceCount = (vertexPerPatchX - 1) * (vertexPerPatchZ - 1);
	indexCount = faceCount * 4;

	CalcBoundY();
	CreateVertexData();
	CreateIndexData();

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexTerrain));
	indexBuffer = new IndexBuffer(indices, indexCount);

	bufferDesc.TexelCellSpaceU = 1.0f / (float)heightMap->GetWidth() - 1;
	bufferDesc.TexelCellSpaceV = 1.0f / (float)heightMap->GetHeight() - 1;
	bufferDesc.TerrainHeightRatio = initDesc.HeightRatio;

	camera = new Fixity();
	perspective = new Perspective(D3D::Width(), D3D::Height(), 0.1f, 1000.0f, Math::PI * 0.35f);

	frustum = new Frustum(NULL, perspective);
}

TerrainLod::~TerrainLod()
{
	delete vertices;
	delete indices;
	delete buffer;

	delete frustum;
	delete camera;
	delete perspective;

	delete heightMap;
	delete baseMap;
	delete layerMap;
	delete alphaMap;
	delete normalMap;
}

void TerrainLod::Update()
{
	Super::Update();

	camera->Update();
	frustum->Update();
	frustum->Planes(bufferDesc.WorldFrustumPlane);

	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
}

void TerrainLod::Render()
{
	Super::Render();

	if (baseMap != NULL)
		sBaseMap->SetResource(baseMap->SRV());

	if (layerMap != NULL && alphaMap != NULL)
	{
		sAlphaMap->SetResource(alphaMap->SRV());
		sLayerMap->SetResource(layerMap->SRV());
	}

	if (normalMap != NULL)
		sNormalMap->SetResource(normalMap->SRV());


	shader->DrawIndexed(0, Pass(), indexCount);
}

void TerrainLod::BaseMap(wstring file)
{
	SafeDelete(baseMap);
	baseMap = new Texture(file);
}

void TerrainLod::LayerMap(wstring layer, wstring alpha)
{
	SafeDelete(alphaMap);
	SafeDelete(layerMap);

	alphaMap = new Texture(alpha);
	layerMap = new Texture(layer);
}

void TerrainLod::NormalMap(wstring file)
{
	SafeDelete(normalMap);

	normalMap = new Texture(file);
}

bool TerrainLod::InBounds(UINT x, UINT z)
{
	return x >= 0 && x < width && z >= 0 && z < height;
}

void TerrainLod::CalcPatchBounds(UINT x, UINT z)
{
	UINT x0 = x * initDesc.CellsPerPatch;
	UINT x1 = (x + 1) * initDesc.CellsPerPatch;

	UINT z0 = z * initDesc.CellsPerPatch;
	UINT z1 = (z + 1) * initDesc.CellsPerPatch;

	float minY = FLT_MAX;
	float maxY = FLT_MIN;

	for (UINT z = z0; z <= z1; z++)
	{
		for (UINT x = x0; x <= x1; x++)
		{
			float y = 0.0f;
			UINT pixel = width * (height - 1 - z) + x;

			if (InBounds(x, z))
				y = heightMapPixel[pixel].b * 255 / initDesc.HeightRatio;

			minY = min(minY, y);
			maxY = max(maxY, y);
		}
	}

	UINT patchID = (vertexPerPatchX - 1) *z + x;
	bounds[patchID] = Vector2(minY, maxY);
}

void TerrainLod::CalcBoundY()
{
	bounds.assign(faceCount, Vector2());// vector에 개수 제한

	for (UINT z = 0; z < vertexPerPatchZ -1 ; z++)
	{
		for (UINT x = 0; x < vertexPerPatchX -1 ; x++)
		{
			CalcPatchBounds(x, z);
		}
	}
}

void TerrainLod::CreateVertexData()
{
}

void TerrainLod::CreateIndexData()
{
}
