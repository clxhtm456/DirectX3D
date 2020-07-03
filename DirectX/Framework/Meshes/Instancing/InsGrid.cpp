#include "Framework.h"
#include "InsGrid.h"

InsGrid * InsGrid::Create(float offsetU, float offsetV)
{
	auto pRet = new InsGrid();
	if (pRet && pRet->Init(offsetU, offsetV))
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

bool InsGrid::Init(float offsetU, float offsetV)
{
	_offsetU = offsetU;
	_offsetV = offsetV;

	CreateBuffer();
	return true;
}

InsGrid::InsGrid()
{

}

InsGrid::~InsGrid()
{

}

void InsGrid::CreateMesh()
{
	
}
