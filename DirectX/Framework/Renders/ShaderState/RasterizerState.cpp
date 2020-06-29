#include "Framework.h"
#include "RasterizerState.h"

RasterizerState::RasterizerState()
	: state(nullptr)
{
	ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
	desc.AntialiasedLineEnable = false;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FillMode = D3D11_FILL_SOLID;

	Changed();
}

RasterizerState::~RasterizerState()
{
	state->Release();
}

void RasterizerState::SetState()
{
	D3D::GetDC()->RSSetState(state);
}

void RasterizerState::FillMode(D3D11_FILL_MODE value)
{
	desc.FillMode = value;
	Changed();
}

void RasterizerState::FrontCounterClockwise(bool val)
{
	desc.FrontCounterClockwise = val;
	Changed();
}

void RasterizerState::Changed()
{
	if (state != nullptr)
		state->Release();

	HRESULT hr = D3D::GetDevice()->CreateRasterizerState(&desc, &state);
	DEBUG(hr);
}