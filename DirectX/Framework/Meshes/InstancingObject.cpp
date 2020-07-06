#include "Framework.h"
#include "InstancingObject.h"

InstancingObject* InstancingObject::Create(Mesh* master)
{
	auto pRet = new InstancingObject();
	if (pRet && pRet->Init(master))
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

bool InstancingObject::Init(Mesh* master)
{
	if (!Super::Init())
		return false;

	if (master == nullptr)
		return false;

	this->master = master;

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

void InstancingObject::Destroy()
{
	//master->
}
