#include "Framework.h"
#include "DirectLight.h"

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
	direction = Vector3(-1, -1, 1);
	specExp = 8.0f;

	ambient = Color(0.1f, 0.1f, 0.1f, 1.0f);

	isSpecularMap = 0;
	isNormalMap = 0;
	return true;
}

DirectionLight::DirectionLight()
{
}

DirectionLight::~DirectionLight()
{
}

void DirectionLight::SetBuffer(OUT LightBuffer* buffer)
{
	buffer->data.direction = direction;
	buffer->data.ambient = ambient;
	buffer->data.specExp = specExp;
	buffer->data.isNormalMap = isNormalMap;
	buffer->data.isSpecularMap = isSpecularMap;
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
}

void DirectionLight::PreRender(Camera* viewer)
{
}

void DirectionLight::PostRender(Camera* viewer)
{
}

void DirectionLight::RemoveFromParent()
{
}
