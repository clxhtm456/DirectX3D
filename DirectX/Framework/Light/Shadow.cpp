#include "Framework.h"
#include "Shadow.h"

Shadow::Shadow(DirectionLight* light, Vector3 position, float radius, UINT width, UINT height):
	_light(light),
	position(position),
	radius(radius),
	width(width),
	height(height)
{
	psDesc.MapSize = Vector2((float)width, (float)height);

	vsBuffer = new ConstantBuffer(&vsDesc, sizeof(VsDesc));
	psBuffer = new ConstantBuffer(&psDesc, sizeof(PsDesc));

	shadowMap = _light->GetDepthSRV();
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

		Check(D3D::GetDevice()->CreateSamplerState(&desc, &shadowSample));
	}
}

Shadow::~Shadow()
{
	delete vsBuffer;
	delete psBuffer;

	shadowSample->Release();
}

void Shadow::Set()
{
	UpdateVolume();

	vsBuffer->SetVSBuffer(2);
	psBuffer->SetPSBuffer(2);

	D3D::GetDC()->PSSetShaderResources(3, 1, &shadowMap);
	D3D::GetDC()->PSSetSamplers(3, 1, &shadowSample);


	ImGui::InputInt("ShadowQuality", (int*)&psDesc.Quality);
	psDesc.Quality %= 4;
}

void Shadow::SetShadowMap(ID3D11ShaderResourceView* srv)
{
	shadowMap = srv;
}

void Shadow::UpdateVolume()
{
	if (_light == NULL)
		return;

	XMVECTOR up = XMVectorSet(0, 1, 0,0);
	XMVECTOR direction = XMLoadFloat3( &_light->GetDirection());//**빛에 대한 depth 를 구함
	XMVECTOR tPosition = direction*radius * -2.0f;

	vsDesc.View = XMMatrixLookAtLH(tPosition, XMLoadFloat3(&position), up);

	XMVECTOR cube;
	cube = XMVector3TransformCoord(XMLoadFloat3(&position), vsDesc.View);

	float left = XMVectorGetX(cube) - radius;
	float bottom = XMVectorGetY(cube) - radius;
	float nea = XMVectorGetZ( cube) - radius;

	float right = XMVectorGetX(cube) + radius;
	float top = XMVectorGetY(cube) + radius;
	float fa = XMVectorGetZ(cube) + radius;

	vsDesc.Projection = XMMatrixOrthographicLH(right - left, top - bottom, nea, fa);
	psDesc.testColor = Vector4(0, 1, 1, 1);

	XMVECTOR test;
	test = XMVector3TransformCoord(XMLoadFloat3(&position), vsDesc.Projection);
}
