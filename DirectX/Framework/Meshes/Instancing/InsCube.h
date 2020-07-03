#pragma once
#include "InsMesh.h"

class InsCube : public InsMesh
{
public:
	static InsCube* Create();
	static InsCube* CreateInstance();
	bool Init();
public:
	InsCube();
	virtual ~InsCube();

private:
	void CreateMesh() override;
public:
	static UINT cubeInstanceIndex;
};
