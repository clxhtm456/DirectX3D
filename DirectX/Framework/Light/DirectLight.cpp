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

void DirectionLight::SetRNShader2Origin(RenderingNode* node)
{
	Super::SetRNShader2Origin(node);

	vsShaderSlot->RecompileVS("VS_Shadow");
	psShaderSlot->RecompilePS("PS_Shadow");

}

void DirectionLight::PostUpdate()
{
}

void DirectionLight::Update()
{
	float angle[3] = { direction.x,direction.y ,direction.z };
	ImGui::SliderFloat3("Light", angle, -1, 1);

	direction = Vector3(angle[0],angle[1],angle[2]);
}

void DirectionLight::LateUpdate()
{
}

void DirectionLight::Render(Camera* viewer)
{
	__super::Render(viewer);
}


void DirectionLight::PostRender(Camera* viewer)
{
}

void DirectionLight::SetUpRender()
{
	__super::SetUpRender();
	shadow->Set();
}
