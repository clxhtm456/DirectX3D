#include "Framework.h"
#include "GBuffer.h"

#define GBUFFERMAP	3

GBuffer::GBuffer(UINT width, UINT height)
{
	this->width = width < 1 ? (UINT)D3D::Width() : width;
	this->height = height < 1 ? (UINT)D3D::Height() : height;


	diffuseRTV = new RenderTarget(this->width, this->height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	specularRTV = new RenderTarget(this->width, this->height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	emissiveRTV = new RenderTarget(this->width, this->height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	normalRTV = new RenderTarget(this->width, this->height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	tangentRTV = new RenderTarget(this->width, this->height, DXGI_FORMAT_R32G32B32A32_FLOAT);

	depthStencil = new DepthStencil(this->width, this->height, true);

	viewport = new Viewport((float)this->width, (float)this->height);

	/*pointLightBuffer = new ConstantBuffer(&pointLightDesc, sizeof(PointLightDesc));
	sPointLightBuffer = shader->AsConstantBuffer("CB_Deffered_PointLight");

	spotLightBuffer = new ConstantBuffer(&spotLightDesc, sizeof(SpotLightDesc));
	sSpotLightBuffer = shader->AsConstantBuffer("CB_Deffered_SpotLight");*/


	CreateDepthStencilView();
	CreateDepthStencilState();
	CreateRasterizerState();

	for (UINT i = 0; i < 6; i++)
	{
		debug2D[i] = new Render2D();
		debug2D[i]->GetTransform()->Position(75 + (float)i * 150, 75, 0);
		debug2D[i]->GetTransform()->Scale(150, 150, 0);
	}
	debug2D[0]->SRV(diffuseRTV->SRV());
	debug2D[1]->SRV(specularRTV->SRV());
	debug2D[2]->SRV(emissiveRTV->SRV());
	debug2D[3]->SRV(normalRTV->SRV());
	debug2D[4]->SRV(tangentRTV->SRV());
	debug2D[5]->SRV(depthStencil->SRV());

}

GBuffer::~GBuffer()
{
	delete diffuseRTV;
	delete specularRTV;
	delete emissiveRTV;
	delete normalRTV;
	delete tangentRTV;
}

void GBuffer::PackGBuffer()
{
	RenderTarget* rtvs[5];
	rtvs[0] = diffuseRTV;
	rtvs[1] = specularRTV;
	rtvs[2] = emissiveRTV;
	rtvs[3] = normalRTV;
	rtvs[4] = tangentRTV;

	RenderTarget::Sets(rtvs, 5, depthStencil);
	viewport->RSSetViewport();

	packDss->SetState();
}

void GBuffer::Render()
{
	D3D::Get()->SetRenderTarget(NULL, depthStencilReadOnly);
	D3D::GetDC()->ClearDepthStencilView(depthStencilReadOnly, D3D11_CLEAR_DEPTH, 1, 0);

	ID3D11ShaderResourceView* srvs[6] =
	{
		depthStencil->SRV(),
		diffuseRTV->SRV(),
		specularRTV->SRV(),
		emissiveRTV->SRV(),
		normalRTV->SRV(),
		tangentRTV->SRV(),
	};

	D3D::GetDC()->PSSetShaderResources(GBUFFERMAP, 1, srvs);

	RenderDirectional();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

	if (bDrawPointLights)
		RenderPointLights();
	if (bDrawSpotLights)
		RenderSpotLights();
}

void GBuffer::DebugRender()
{
}

void GBuffer::CreateDepthStencilView()
{
	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;
	desc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;

	Check(D3D::GetDevice()->CreateDepthStencilView(depthStencil->BackBuffer(), &desc, &depthStencilReadOnly));
}

void GBuffer::CreateDepthStencilState()
{
	//PackDSS
	const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = {
		D3D11_STENCIL_OP_REPLACE,
		D3D11_STENCIL_OP_REPLACE,
		D3D11_STENCIL_OP_REPLACE,
		D3D11_COMPARISON_ALWAYS
	};

	packDss = new DepthStencilState();
	packDss->FrontFaceOp(stencilMarkOp);
	packDss->BackFaceOp(stencilMarkOp);

	//No Depth Write Less
	const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp2 =
	{
		D3D11_STENCIL_OP_KEEP,
		D3D11_STENCIL_OP_KEEP,
		D3D11_STENCIL_OP_KEEP,
		D3D11_COMPARISON_EQUAL
	};
	noDepthWriteLessDSS = new DepthStencilState();
	noDepthWriteLessDSS->DepthWriteMask(D3D11_DEPTH_WRITE_MASK_ZERO);
	noDepthWriteLessDSS->FrontFaceOp(stencilMarkOp2);
	noDepthWriteLessDSS->BackFaceOp(stencilMarkOp2);

	//No Depth Write Greater
	noDepthWriteGreaterDSS = new DepthStencilState();
	noDepthWriteGreaterDSS->DepthFunc(D3D11_COMPARISON_GREATER_EQUAL);
}

void GBuffer::CreateRasterizerState()
{
}

void GBuffer::RenderDirectional()
{
}

void GBuffer::CalcPointLights(UINT count)
{
}

void GBuffer::RenderPointLights()
{
}

void GBuffer::CalcSpotLights(UINT count)
{
}

void GBuffer::RenderSpotLights()
{
}
