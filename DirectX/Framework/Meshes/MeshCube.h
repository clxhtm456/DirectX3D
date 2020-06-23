#pragma once
#include "Mesh.h"

class MeshCube : public Mesh
{
public:
	static MeshCube* Create();
public:
	MeshCube();
	virtual ~MeshCube();

private:
	void CreateMesh() override;
};
