#include "Framework.h"
#include "RenderingNode.h"

RenderingNode::RenderingNode() : Node()
{
	worldBuffer = new WorldBuffer();
	lightBuffer = new LightBuffer();
	rasterizerState = new RasterizerState();
	
}

RenderingNode::~RenderingNode()
{
	delete worldBuffer;
	delete lightBuffer;
	delete rasterizerState;
}


void RenderingNode::Start()
{
	Super::Start();

	CalcWorldMatrix();
}

void RenderingNode::PostUpdate()
{
}

void RenderingNode::Update()
{
	if (_scene != nullptr && _scene->GetDirectionLight() != nullptr)
	{
		_scene->GetDirectionLight()->SetBuffer(lightBuffer);
	}
}

void RenderingNode::LateUpdate()
{
}

void RenderingNode::Render(Camera* viewer)
{
	if (shadowMap != NULL)
	{
		D3D::GetDC()->PSSetShaderResources(10, 1, &shadowMap);
	}
}


void RenderingNode::PostRender(Camera* viewer)
{
}

void RenderingNode::Draw(Camera * viewer)
{
	VPSet(viewer);
	WorldSet();

	LightSet();

	if (vertexBuffer != NULL && indexBuffer != NULL)
	{
		vertexBuffer->Render();
		indexBuffer->Render();
		rasterizerState->SetState();

		if (vsShader == NULL && psShader == NULL)
			shader->Render();
		else
		{
			vsShader->RenderVS();
			psShader->RenderPS();
		}
		


		D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	}
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

void RenderingNode::SetShader(wstring file, string vs, string ps)
{
	Shader* nShader = Shader::Add(file,vs,ps);

	SetShader(nShader);
}

void RenderingNode::SetPSShader(wstring file, string ps)
{
	Shader* nShader = Shader::PSAdd(file,ps);

	SetPSShader(nShader);
}

void RenderingNode::SetVSShader(wstring file, string vs)
{
	Shader* nShader = Shader::VSAdd(file,vs);

	SetVSShader(nShader);
}

void RenderingNode::SetShader(Shader* nShader)
{
	shader = nShader;
	vsShader = NULL;
	psShader = NULL;
}

void RenderingNode::SetPSShader(Shader* nShader)
{
	if (vsShader == NULL)
		vsShader = shader;
	psShader = nShader;

	//변경된 psshader가 vsshader와 같을경우 통합
	if (psShader == vsShader)
		SetShader(nShader);
}

void RenderingNode::SetVSShader(Shader* nShader)
{
	if (psShader == NULL)
		psShader = shader;
	vsShader = nShader;

	//변경된 vsshader가 psshader와 같을경우 통합
	if (psShader == vsShader)
		SetShader(nShader);
}

Shader* RenderingNode::GetVSShader()
{
	if (vsShader == NULL)
		return shader;
	else
		return vsShader;
}

Shader* RenderingNode::GetPSShader()
{
	if (psShader == NULL)
		return shader;
	else
		return psShader;
}

RasterizerState * RenderingNode::GetRasterizerState()
{
	return rasterizerState;
}

void RenderingNode::WorldSet()
{
	worldBuffer->SetVSBuffer(VS_WORLDBUFFER);
}

void RenderingNode::VPSet(Camera* viewer)
{
	if (viewer != nullptr)
		viewer->GetVPBuffer()->SetVSBuffer(VS_VPBUFFER);
}

void RenderingNode::LightSet()
{
	lightBuffer->SetPSBuffer(PS_GLOBALLIGHTBUFFER);
}

void RenderingNode::CalcWorldMatrix()
{
	worldBuffer->SetWorld(GetWorld());
}
