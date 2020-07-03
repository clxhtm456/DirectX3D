#pragma once
#include "InsMesh.h"

class InsSphere : public InsMesh
{
public:
	static InsSphere* Create(float radius, UINT stackCount = 20, UINT sliceCount = 20);
	bool Init(float radius, UINT stackCount, UINT sliceCount);
public:
	InsSphere();
	~InsSphere();

protected:
	void CreateMesh() override;

private:
	float _radius;

	UINT _stackCount;
	UINT _sliceCount;
};