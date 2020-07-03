#include "Framework.h"
#include "InsCylinder.h"

InsCylinder * InsCylinder::Create(float radius, float height, UINT sliceCount, UINT stackCount)
{
	auto pRet = new InsCylinder();
	if (pRet && pRet->Init(radius,height,sliceCount,stackCount))
	{
		pRet->AutoRelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

bool InsCylinder::Init(float radius, float height, UINT sliceCount, UINT stackCount)
{
	_topRadius = radius;
	_bottomRadius = radius;
	_height = height;
	_sliceCount = sliceCount;
	_stackCount = stackCount;

	CreateBuffer();
	return true;
}

InsCylinder::InsCylinder()
{

}

InsCylinder::~InsCylinder()
{

}

void InsCylinder::CreateMesh()
{
	
}

void InsCylinder::BuildTopCap(vector<MeshVertex>& vertices, vector<UINT>& indices)
{
	
}

void InsCylinder::BuildBottomCap(vector<MeshVertex>& vertices, vector<UINT>& indices)
{
	
}