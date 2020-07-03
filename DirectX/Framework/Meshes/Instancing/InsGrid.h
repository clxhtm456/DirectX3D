#pragma once
#include "InsMesh.h"

class InsGrid : public InsMesh
{
public:
	static InsGrid* Create(float offsetU = 1.0f, float offsetV = 1.0f);
	bool Init(float offsetU, float offsetV);
public:
	InsGrid();
	~InsGrid();

private:
	void CreateMesh() override;

	float _offsetU;
	float _offsetV;
};
