#include "Framework.h"
#include "InsSphere.h"

InsSphere * InsSphere::Create(float radius, UINT stackCount, UINT sliceCount)
{
	auto pRet = new InsSphere();
	if (pRet && pRet->Init(radius,stackCount, sliceCount))
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

InsSphere::InsSphere()
{

}

bool InsSphere::Init(float radius, UINT stackCount, UINT sliceCount)
{
	_radius = radius;
	_stackCount = stackCount;
	_sliceCount = sliceCount;

	CreateBuffer();
	return true;
}

InsSphere::~InsSphere()
{

}

void InsSphere::CreateMesh()
{
}
