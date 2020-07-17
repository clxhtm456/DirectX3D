#include "Framework.h"
#include "DirectLight.h"
#include "Shadow.h"

DirectionLight* DirectionLight::Create()
{
	auto pRet = new DirectionLight();
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

bool DirectionLight::Init()
{
	shadow = new Shadow(this,Vector3(0,0,0),100);

	return true;
}

DirectionLight::DirectionLight()
{
}

DirectionLight::~DirectionLight()
{
	delete shadow;
}

void DirectionLight::SetBuffer(OUT LightBuffer* buffer)
{
	buffer->data.direction = direction;
	buffer->data.ambient = ambient;
	buffer->data.Specular = specular;
	buffer->data.position = position;
}


void DirectionLight::SetRNShader2Depth(RenderingNode * node)
{
	Super::SetRNShader2Depth(node);
	node->GetRasterizerState()->FrontCounterClockwise(true);
}

void DirectionLight::SetRNShader2Origin(RenderingNode* node)
{
	Super::SetRNShader2Origin(node);

	node->GetRasterizerState()->FrontCounterClockwise(false);
	if (useShadow == false)
		return;

	if (node->GetUseShadow() == false)
		return;

	vsShaderSlot->RecompileVS("VS");
	vsShaderSlot->RecompilePS("PS");
	node->SetShadowMap(depthStencil->SRV());
	

}

void DirectionLight::PostUpdate()
{
}

void DirectionLight::Update()
{
	float angle[3] = { direction.x,direction.y ,direction.z };
	ImGui::SliderFloat3("LightRot", angle, -1, 1);

	float lightPos[3] = { position.x,position.y ,position.z };
	ImGui::SliderFloat3("LightPos", lightPos, -100, 100);

	direction = Vector3(angle[0],angle[1],angle[2]);
	position = Vector3(lightPos[0], lightPos[1], lightPos[2]);
}

void DirectionLight::LateUpdate()
{
}

void DirectionLight::Render(Camera* viewer)
{
	__super::Render(viewer);
	shadow->Draw();
}


void DirectionLight::PostRender(Camera* viewer)
{
}

void DirectionLight::SetUpRender()
{
	__super::SetUpRender();
	shadow->Set();
}
