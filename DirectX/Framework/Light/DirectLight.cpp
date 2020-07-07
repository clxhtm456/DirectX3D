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

	buffer->data.lightView = lightView;
	buffer->data.lightProjection = lightProjection;
}

void DirectionLight::CalcLightVP()
{
	float radius = 100;
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR vDirection = XMLoadFloat3(&direction);//**빛에 대한 depth 를 구함
	XMVECTOR tPosition = vDirection * radius * -2.0f;

	lightView = XMMatrixLookAtLH(tPosition, XMLoadFloat3(&position), up);

	XMVECTOR cube;
	cube = XMVector3TransformCoord(XMLoadFloat3(&position), lightView);

	float left = XMVectorGetX(cube) - radius;
	float bottom = XMVectorGetY(cube) - radius;
	float nea = XMVectorGetZ(cube) - radius;

	float right = XMVectorGetX(cube) + radius;
	float top = XMVectorGetY(cube) + radius;
	float fa = XMVectorGetZ(cube) + radius;

	lightProjection = XMMatrixOrthographicLH(right - left, top - bottom, nea, fa);

	lightView = XMMatrixTranspose(lightView);
	lightProjection = XMMatrixTranspose(lightProjection);
}

void DirectionLight::SetRNShader2Origin(RenderingNode* node)
{
	Super::SetRNShader2Origin(node);

	if (useShadow == false)
		return;

	vsShaderSlot->RecompileVS("VS_Shadow");
	psShaderSlot->RecompilePS("PS_Shadow");

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
	CalcLightVP();
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
