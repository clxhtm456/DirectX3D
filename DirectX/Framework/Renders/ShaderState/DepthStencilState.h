#pragma once

class DepthStencilState
{
private:
	D3D11_DEPTH_STENCIL_DESC desc;
	ID3D11DepthStencilState* state;

public:
	DepthStencilState();
	~DepthStencilState();

	void SetState(UINT stencilRef = 1);

	void DepthEnable(bool val);

	void FrontFaceOp(D3D11_DEPTH_STENCILOP_DESC op);
	void BackFaceOp(D3D11_DEPTH_STENCILOP_DESC op);
	void DepthWriteMask(D3D11_DEPTH_WRITE_MASK target);
	void DepthFunc(D3D11_COMPARISON_FUNC func);

	void Changed();
};