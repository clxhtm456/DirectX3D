#include "Framework.h"
#include "RenderingNode.h"

RenderingNode::RenderingNode() : Node()
{
	worldBuffer = new WorldBuffer();
}

RenderingNode::~RenderingNode()
{
	delete worldBuffer;
}

bool RenderingNode::Init()
{
	return true;
}

void RenderingNode::PostUpdate()
{
}

void RenderingNode::Update()
{
	Matrix S,R, T;
	S = XMMatrixScaling(XMVectorGetX(_scale), XMVectorGetY(_scale), XMVectorGetZ(_scale));
	R = XMMatrixRotationRollPitchYaw(XMVectorGetX(_rotation), XMVectorGetY(_rotation), XMVectorGetZ(_rotation));
	T = XMMatrixTranslation(XMVectorGetX(_position), XMVectorGetY(_position), XMVectorGetZ(_position));

	worldBuffer->SetWorld(S*R*T);
}

void RenderingNode::LateUpdate()
{
}

void RenderingNode::Render()
{
	worldBuffer->SetVSBuffer(1);
	Context::Get()->GetMainCamera()->GetVPBuffer()->SetVSBuffer(0);
}

void RenderingNode::PreRender()
{
}

void RenderingNode::PostRender()
{
}

void RenderingNode::RemoveFromParent()
{
}
