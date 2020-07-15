#include "Framework.h"
#include "EmptyNode.h"

EmptyNode* EmptyNode::Create()
{
	auto pRet = new EmptyNode();
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

bool EmptyNode::Init()
{
	if (!Super::Init())
		return false;

	return true;
}

EmptyNode::EmptyNode()
{
}

EmptyNode::~EmptyNode()
{
}


void EmptyNode::PostUpdate()
{
}

void EmptyNode::Update()
{
}

void EmptyNode::LateUpdate()
{
}

void EmptyNode::ResourceBinding(Camera* viewer)
{
}

void EmptyNode::PostRender(Camera* viewer)
{
}