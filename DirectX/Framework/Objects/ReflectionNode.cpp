#include "Framework.h"
#include "ReflectionNode.h"

ReflectionNode::ReflectionNode(bool isDeffered)
	:isDeffered(isDeffered)
{
	depthShader = Shader::Add(L"DepthStencil");

	depthStencil = new DepthStencil(1024, 1024);
	viewport = new Viewport((float)1024, (float)1024);

	renderTarget = new RenderTarget(1024,1024);
}

ReflectionNode::~ReflectionNode()
{
	delete renderTarget;
	delete depthStencil;
	delete viewport;
}



void ReflectionNode::SetRNShader2Depth(RenderingNode* node)
{

	vsShaderSlot = node->GetVSShader();
	psShaderSlot = node->GetVSShader();
	node->SetShader(depthShader);
}

void ReflectionNode::SetRNShader2Origin(RenderingNode* node)
{

	node->SetVSShader(vsShaderSlot);
	node->SetPSShader(psShaderSlot);
}

void ReflectionNode::PostUpdate()
{
}

void ReflectionNode::Update()
{
}

void ReflectionNode::LateUpdate()
{
}

void ReflectionNode::SetUpRender()
{
	renderTarget->Set(depthStencil);
	viewport->RSSetViewport();
}

void ReflectionNode::Render(Camera* viewer)
{
}


void ReflectionNode::PostRender(Camera* viewer)
{
}


DepthStencil* ReflectionNode::GetDepth()
{
	return depthStencil;
}

RenderTarget* ReflectionNode::GetRenderTarget()
{
	return renderTarget;
}
