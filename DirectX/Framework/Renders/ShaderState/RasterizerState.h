#pragma once

class RasterizerState
{
private:
	D3D11_RASTERIZER_DESC desc;
	ID3D11RasterizerState* state = NULL;

public:
	RasterizerState();
	~RasterizerState();

	void SetState();

	void FillMode(D3D11_FILL_MODE value);
	void FrontCounterClockwise(bool val);

	void Changed();
};