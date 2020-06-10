#include "Framework.h"
#include "PerFrame.h"

PerFrame::PerFrame(Shader * shader)
	: shader(shader)
{
	buffer = new ConstantBuffer(&bufferDesc, sizeof(BufferDesc));
	sBuffer = shader->AsConstantBuffer("CB_PerFrame");
	ZeroMemory(bufferDesc.Culling, sizeof(Plane) * 4);
	ZeroMemory(bufferDesc.Cliping, sizeof(Plane));

	lightBuffer = new ConstantBuffer(&lightDesc, sizeof(LightDesc));
	sLightBuffer = shader->AsConstantBuffer("CB_Light");

	pointLightBuffer = new ConstantBuffer(&pointLightDesc, sizeof(PointLightDesc));
	sPointLightBuffer = shader->AsConstantBuffer("CB_PointLights");

	spotLightBuffer = new ConstantBuffer(&spotLightDesc, sizeof(SpotLightDesc));
	sSpotLightBuffer = shader->AsConstantBuffer("CB_SpotLights");

	fogBuffer = new ConstantBuffer(&fogDesc, sizeof(FogDesc));
	sFogBuffer = shader->AsConstantBuffer("CB_Fog");
}

PerFrame::~PerFrame()
{
	SafeDelete(buffer);
	SafeDelete(lightBuffer);
	SafeDelete(pointLightBuffer);
	SafeDelete(spotLightBuffer);
}

void PerFrame::Update()
{
	bufferDesc.Time = Time::Get()->Running();

	fogDesc.FogColor = Context::Get()->FogColor();
	fogDesc.FogDistance = Context::Get()->FogDistance();
	fogDesc.FogDensity = Context::Get()->FogDensity();
	fogDesc.FogType = Context::Get()->FogType();

	lightDesc.Ambient = Context::Get()->Ambient();
	lightDesc.Specular = Context::Get()->Specular();
	lightDesc.Direction = Context::Get()->Direction();
	lightDesc.Position = Context::Get()->Position();

	pointLightDesc.Count = Context::Get()->PointLights(pointLightDesc.Lights);
	spotLightDesc.Count = Context::Get()->SpotLights(spotLightDesc.Lights);
}

void PerFrame::Render()
{
	bufferDesc.View = Context::Get()->View();
	D3DXMatrixInverse(&bufferDesc.ViewInverse, NULL, &bufferDesc.View);

	bufferDesc.Projection = Context::Get()->Projection();
	D3DXMatrixInverse(&bufferDesc.ProjectionInverse, NULL, &bufferDesc.Projection);

	bufferDesc.VP = bufferDesc.View * bufferDesc.Projection;

	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	lightBuffer->Apply();
	sLightBuffer->SetConstantBuffer(lightBuffer->Buffer());

	pointLightBuffer->Apply();
	sPointLightBuffer->SetConstantBuffer(pointLightBuffer->Buffer());

	spotLightBuffer->Apply();
	sSpotLightBuffer->SetConstantBuffer(spotLightBuffer->Buffer());

	fogBuffer->Apply();
	sFogBuffer->SetConstantBuffer(fogBuffer->Buffer());
}
