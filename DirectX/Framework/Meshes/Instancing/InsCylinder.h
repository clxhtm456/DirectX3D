#pragma once
#include "InsMesh.h"

class InsCylinder : public InsMesh
{
public:
	static InsCylinder* Create(float radius, float height, UINT sliceCount = 10, UINT stackCount = 10);
	bool Init(float radius, float height, UINT sliceCount, UINT stackCount);
public:
	InsCylinder();
	~InsCylinder();

protected:
	void CreateMesh() override;

private:
	void BuildTopCap(vector<MeshVertex>& vertices, vector<UINT>& indices);
	void BuildBottomCap(vector<MeshVertex>& vertices, vector<UINT>& indices);

private:
	float _topRadius;
	float _bottomRadius;
	float _height;

	UINT _sliceCount; //Á¤¹Ðµµ
	UINT _stackCount;
};