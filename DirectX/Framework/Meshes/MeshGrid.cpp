#include "Framework.h"
#include "MeshGrid.h"

MeshGrid * MeshGrid::Create(float offsetU, float offsetV)
{
	auto pRet = new MeshGrid();
	if (pRet && pRet->Init(offsetU, offsetV))
	{
		pRet->AutoRelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

bool MeshGrid::Init(float offsetU, float offsetV)
{
	_offsetU = offsetU;
	_offsetV = offsetV;

	CreateBuffer();
	return true;
}

MeshGrid::MeshGrid()
{

}

MeshGrid::~MeshGrid()
{

}

void MeshGrid::CreateMesh()
{
	UINT countX = 11;
	UINT countZ = 11;

	float w = ((float)countX - 1) * 0.5f;
	float h = ((float)countZ - 1) * 0.5f;

	vector<MeshVertex> v;
	for (UINT z = 0; z < countZ; z++)
	{
		for (UINT x = 0; x < countX; x++)
		{
			MeshVertex vertex;
			vertex.Position = Vector3((float)x - w, 0, (float)z - h);
			vertex.Normal = Vector3(0, 1, 0);
			vertex.Tangent = Vector3(1, 0, 0);
			vertex.Uv.x = (float)x / (float)(countX - 1) * _offsetU;
			vertex.Uv.y = (float)z / (float)(countZ - 1) * _offsetV;

			v.push_back(vertex);
		}
	}


	vertices = new MeshVertex[v.size()];
	vertexCount = v.size();
	copy
	(
		v.begin(), v.end(),
		stdext::checked_array_iterator<MeshVertex *>(vertices, vertexCount)
	);


	vector<UINT> indices;
	for (UINT z = 0; z < countZ - 1; z++)
	{
		for (UINT x = 0; x < countX - 1; x++)
		{
			indices.push_back(countX * z + x);
			indices.push_back(countX * (z + 1) + x);
			indices.push_back(countX * z + x + 1);

			indices.push_back(countX * z + x + 1);
			indices.push_back(countX * (z + 1) + x);
			indices.push_back(countX * (z + 1) + x + 1);
		}
	}

	this->indices = new UINT[indices.size()];
	indexCount = indices.size();
	copy
	(
		indices.begin(), indices.end(),
		stdext::checked_array_iterator<UINT *>(this->indices, indexCount)
	);
}
