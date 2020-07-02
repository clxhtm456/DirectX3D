#pragma once
#include "Mesh.h"
#include "Terrain.h"

class MeshCube : public Mesh , public TerrainObject
{
public:
	static MeshCube* Create();
	static MeshCube* CreateInstance();
	bool Init();
public:
	MeshCube();
	virtual ~MeshCube();

private:
	void CreateMesh() override;

	// TerrainObject을(를) 통해 상속됨
	virtual wstring ObjectType() override;
};
