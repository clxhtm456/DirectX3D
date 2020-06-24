#pragma once
#include "Mesh.h"

class MeshSphere : public Mesh
{
public:
	static MeshSphere* Create(float radius, UINT stackCount = 20, UINT sliceCount = 20);
	bool Init(float radius, UINT stackCount, UINT sliceCount);
public:
	MeshSphere();
	~MeshSphere();

protected:
	void CreateMesh() override;

private:
	float _radius;

	UINT _stackCount;
	UINT _sliceCount;
};