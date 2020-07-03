#pragma once

#include "InsMesh.h"

class InsQuad : public InsMesh
{
public:
	static InsQuad* Create();
	bool Init();
public:
	InsQuad();
	~InsQuad();

private:
	void CreateMesh() override;

};
