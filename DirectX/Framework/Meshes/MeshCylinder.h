#pragma once
#include "Mesh.h"

class MeshCylinder : public Mesh
{
public:
	static MeshCylinder* Create(float radius, float height, UINT sliceCount = 10, UINT stackCount = 10);
	bool Init(float radius, float height, UINT sliceCount, UINT stackCount);
public:
	MeshCylinder();
	~MeshCylinder();

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