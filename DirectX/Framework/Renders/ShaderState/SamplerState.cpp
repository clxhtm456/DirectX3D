#include "Framework.h"
#include "SamplerState.h"

SamplerState::SamplerState()
	: state(nullptr), desc{}
{
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	
	Changed();
}

SamplerState::~SamplerState()
{
	state->Release();
}

void SamplerState::Set(UINT slot)
{
	D3D::GetDC()->PSSetSamplers(slot, 1, &state);
}

void SamplerState::ComparisonFunc(D3D11_COMPARISON_FUNC val)
{
	desc.ComparisonFunc = val;
	Changed();
}

void SamplerState::Fillter(D3D11_FILTER val)
{
	desc.Filter = val;
	Changed();
}

void SamplerState::Address(D3D11_TEXTURE_ADDRESS_MODE val)
{
	desc.AddressU = val;
	desc.AddressV = val;
	Changed();
}

void SamplerState::Changed()
{
	if (state != nullptr)
		state->Release();

	HRESULT hr = D3D::GetDevice()->CreateSamplerState(&desc, &state);
	DEBUG(hr);
}
