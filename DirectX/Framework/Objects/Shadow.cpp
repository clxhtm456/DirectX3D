#include "Framework.h"
#include "Shadow.h"

Shadow::Shadow(Shader * shader, Vector3 position, float radius, UINT width, UINT height):
	shader(shader),
	position(position),
	radius(radius),
	width(width),
	height(height)
{
	renderTarget = new RenderTarget(width, height);
	depthStencil = new DepthStencil(width, height);
	viewport = new Viewport((float)width, (float)height);
	desc.MapSize = Vector2((float)width, (float)height);

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Shadow");
	sShadowMap = shader->AsSRV("ShadowMap");

	//Create SamplerState
	{
		//CD3D11_SAMPLER_DESC//이미 생성되있는 샘플
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
		desc.Filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.MaxAnisotropy = 1;//비등방성 필터링
		desc.MaxLOD = FLT_MAX;

		Check(D3D::GetDevice()->CreateSamplerState(&desc, &samplerState));
		sSamplerState = shader->AsSampler("ShadowSampler");
	}
}

Shadow::~Shadow()
{
	delete buffer;
	delete renderTarget;
	delete depthStencil;
	delete viewport;

	samplerState->Release();
}

void Shadow::Set()
{
	renderTarget->Set(depthStencil);
	viewport->RSSetViewport();

	UpdateVolume();

	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	//ImGui::SliderFloat("Bias", &desc.Bias, -0.1f, 1.0f,"%.4f");

	sShadowMap->SetResource(depthStencil->SRV());
	sSamplerState->SetSampler(0, samplerState);

	ImGui::InputInt("ShadowQuality", (int*)&desc.Quality);
	desc.Quality %= 4;
}

void Shadow::UpdateVolume()
{
	Vector3 up = Vector3(0, 1, 0);
	Vector3 direction = Context::Get()->Direction();//**빛에 대한 depth 를 구함
	Vector3 position = direction*radius * -2.0f;

	D3DXMatrixLookAtLH(&desc.View, &position, &this->position, &up);

	Vector3 cube;
	D3DXVec3TransformCoord(&cube, &this->position, &desc.View);

	float left = cube.x - radius;
	float bottom = cube.y - radius;
	float nea = cube.z - radius;

	float right = cube.x + radius;
	float top = cube.y + radius;
	float fa = cube.z + radius;

	D3DXMatrixOrthoLH(&desc.Projection, right - left, top - bottom, nea, fa);
}
