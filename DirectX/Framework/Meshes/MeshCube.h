#pragma once
#include "Mesh.h"
#include "Terrain.h"

class MeshCube : public Mesh
{
public:
	static MeshCube* Create();
	bool Init();
public:
	MeshCube();
	virtual ~MeshCube();

private:
	void CreateMesh() override;

};
