#pragma once

#include "Mesh.h"

class MeshQuad : public Mesh
{
public:
	static MeshQuad* Create();
	bool Init();
public:
	MeshQuad();
	~MeshQuad();

private:
	void CreateMesh() override;

};
