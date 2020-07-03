#include "Framework.h"
#include "InsCube.h"

UINT InsCube::cubeInstanceIndex = 0;

InsCube* InsCube::Create()
{
	InsCube* pRet = new InsCube();
	if (pRet && pRet->Init())
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

bool InsCube::Init()
{
	CreateBuffer();
	return true;
}

InsCube::InsCube()
{

}

InsCube::~InsCube()
{

}

void InsCube::CreateMesh()
{
}

