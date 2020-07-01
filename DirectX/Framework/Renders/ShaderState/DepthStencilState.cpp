#include "Framework.h"
#include "DepthStencilState.h"

DepthStencilState::DepthStencilState()
	: state(nullptr)
{
	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;

	desc.StencilEnable = true;
	desc.StencilReadMask = 0xff;
	desc.StencilWriteMask = 0xff;

	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	Changed();
}

DepthStencilState::~DepthStencilState()
{
	state->Release();
}

void DepthStencilState::SetState(UINT stencilRef)
{
	D3D::GetDC()->OMSetDepthStencilState(state, stencilRef);
}

void DepthStencilState::DepthEnable(bool val)
{
	desc.DepthEnable = val;
	Changed();
}

void DepthStencilState::FrontFaceOp(D3D11_DEPTH_STENCILOP_DESC op)
{
	desc.FrontFace = op;
	Changed();
}

void DepthStencilState::BackFaceOp(D3D11_DEPTH_STENCILOP_DESC op)
{
	desc.BackFace = op;
	Changed();
}

void DepthStencilState::DepthWriteMask(D3D11_DEPTH_WRITE_MASK target)
{
	desc.DepthWriteMask = target;
	Changed();
}

void DepthStencilState::DepthFunc(D3D11_COMPARISON_FUNC func)
{
	desc.DepthFunc = func;
	Changed();
}

void DepthStencilState::Changed()
{

	if (state != nullptr)
		state->Release();

	HRESULT hr = D3D::GetDevice()->CreateDepthStencilState(&desc, &state);
	DEBUG(hr);
}
