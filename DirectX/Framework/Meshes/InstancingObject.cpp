#include "Framework.h"
#include "InstancingObject.h"

InstancingObject* InstancingObject::Create()
{
	auto pRet = new InstancingObject();
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

bool InstancingObject::Init()
{
	if (!Super::Init())
		return false;

	return true;
}

InstancingObject::InstancingObject()
{
}

InstancingObject::~InstancingObject()
{
}


void InstancingObject::PostUpdate()
{
}

void InstancingObject::Update()
{
}

void InstancingObject::LateUpdate()
{
}

void InstancingObject::Render(Camera* viewer)
{
}

void InstancingObject::PostRender(Camera* viewer)
{
}