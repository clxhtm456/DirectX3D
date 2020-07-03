#include "Framework.h"
#include "InsQuad.h"

InsQuad * InsQuad::Create()
{
	auto pRet = new InsQuad();
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

bool InsQuad::Init()
{
	CreateBuffer();
	return true;
}

InsQuad::InsQuad()
{

}

InsQuad::~InsQuad()
{

}

void InsQuad::CreateMesh()
{
}
