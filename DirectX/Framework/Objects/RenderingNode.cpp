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


void RenderingNode::PostUpdate()
{
}

void RenderingNode::Update()
{
	Matrix S,R, T;
	S = XMMatrixScaling(XMVectorGetX(_scale), XMVectorGetY(_scale), XMVectorGetZ(_scale));
	R = XMMatrixRotationRollPitchYaw(XMVectorGetX(_rotation), XMVectorGetY(_rotation), XMVectorGetZ(_rotation));
	T = XMMatrixTranslation(XMVectorGetX(_position), XMVectorGetY(_position), XMVectorGetZ(_position));

	_world = S * R * T;
	worldBuffer->SetWorld(_world);
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

Matrix RenderingNode::GetWorld()
{
	return _world;
}
