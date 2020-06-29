#include "Framework.h"
#include "RenderingNode.h"

RenderingNode::RenderingNode() : Node()
{
	worldBuffer = new WorldBuffer();
	lightBuffer = new LightBuffer();
}

RenderingNode::~RenderingNode()
{
	delete worldBuffer;
	delete lightBuffer;
}


void RenderingNode::Start()
{
	CalcWorldMatrix();
}

void RenderingNode::PostUpdate()
{
}

void RenderingNode::Update()
{
	
}

void RenderingNode::LateUpdate()
{
}

void RenderingNode::Render(Camera* viewer)
{
	VPSet(viewer);
	WorldSet();

	LightSet();
}

void RenderingNode::PreRender(Camera* viewer)
{
}

void RenderingNode::PostRender(Camera* viewer)
{
}

void RenderingNode::RemoveFromParent()
{
}

void RenderingNode::SetPosition(Vector3 position)
{
	Node::SetPosition(position);

	CalcWorldMatrix();
}

void RenderingNode::SetRotation(Vector3 rotation)
{
	Node::SetRotation(rotation);

	CalcWorldMatrix();
}

void RenderingNode::SetRotationDegree(Vector3 rotation)
{
	Node::SetRotationDegree(rotation);

	CalcWorldMatrix();
}

void RenderingNode::SetScale(Vector3 scale)
{
	Node::SetScale(scale);

	CalcWorldMatrix();
}

void RenderingNode::SetPosition(float x, float y, float z)
{
	Vector3 temp = Vector3(x, y, z);
	SetPosition(temp);
}

void RenderingNode::SetRotation(float x, float y, float z)
{
	Vector3 temp = Vector3(x, y, z);
	SetRotation(temp);
}

void RenderingNode::SetRotationDegree(float x, float y, float z)
{
	Vector3 temp = Vector3(x, y, z);
	SetRotationDegree(temp);
}

void RenderingNode::SetScale(float x, float y, float z)
{
	Vector3 temp = Vector3(x, y, z);
	SetScale(temp);
}

void RenderingNode::SetShader(wstring file)
{
	shader = Shader::Add(file);
}

void RenderingNode::WorldSet()
{
	worldBuffer->SetVSBuffer(1);
}

void RenderingNode::VPSet(Camera* viewer)
{
	if (viewer != nullptr)
		viewer->GetVPBuffer()->SetVSBuffer(0);
}

void RenderingNode::LightSet()
{
	lightBuffer->SetPSBuffer(0);
}

void RenderingNode::CalcWorldMatrix()
{
	Matrix S, R, T;
	S = XMMatrixScaling(XMVectorGetX(_scale), XMVectorGetY(_scale), XMVectorGetZ(_scale));
	R = XMMatrixRotationRollPitchYaw(XMVectorGetX(_rotation), XMVectorGetY(_rotation), XMVectorGetZ(_rotation));
	T = XMMatrixTranslation(XMVectorGetX(_position), XMVectorGetY(_position), XMVectorGetZ(_position));

	_world = S * R * T;
	worldBuffer->SetWorld(_world);

	if (_scene != nullptr)
	{
		_scene->GetDirectionLight()->SetBuffer(lightBuffer);
	}
	
}

Matrix RenderingNode::GetWorld()
{
	return _world;
}
