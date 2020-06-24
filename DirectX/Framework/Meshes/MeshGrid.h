#pragma once
#include "Mesh.h"

class MeshGrid : public Mesh
{
public:
	static MeshGrid* Create(float offsetU = 1.0f, float offsetV = 1.0f);
	bool Init(float offsetU, float offsetV);
public:
	MeshGrid();
	~MeshGrid();

private:
	void CreateMesh() override;

	float _offsetU;
	float _offsetV;
};
