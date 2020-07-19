#include "Framework.h"
#include "GBuffer.h"
#include "Renders/Render2D.h"

#define GBUFFERMAP	9

GBuffer::GBuffer(UINT width, UINT height)
{
	this->width = width < 1 ? (UINT)D3D::Width() : width;
	this->height = height < 1 ? (UINT)D3D::Height() : height;


	deffredShader = Shader::PSAdd(L"DefferedRender");
	gbufferShader = Shader::Add(L"GBufferRender");

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
	RSS = new RasterizerState();

	CreateDepthStencilView();
	CreateDepthStencilState();
	CreateRasterizerState();

	for (UINT i = 0; i < 6; i++)
	{
		debug2D[i] = Render2D::Create();
		debug2D[i]->Retain();
		debug2D[i]->SetPosition(75 + (float)i * 150, 75, 0);
		debug2D[i]->SetScale(150, 150, 0);
	}
	debug2D[0]->SetSRV(depthStencil->SRV());
	debug2D[1]->SetSRV(diffuseRTV->SRV());
	debug2D[2]->SetSRV(specularRTV->SRV());
	debug2D[3]->SetSRV(emissiveRTV->SRV());
	debug2D[4]->SetSRV(normalRTV->SRV());
	debug2D[5]->SetSRV(tangentRTV->SRV());

}

GBuffer::~GBuffer()
{
	delete diffuseRTV;
	delete specularRTV;
	delete emissiveRTV;
	delete normalRTV;
	delete tangentRTV;
	delete depthStencil;
	delete viewport;

	delete RSS;
	delete packDss;
	delete noDepthWriteLessDSS;
	delete noDepthWriteGreaterDSS;

	for (UINT i = 0; i < 6; i++)
		debug2D[i]->Release();
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

void GBuffer::Render(Camera* viewer)
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

	D3D::GetDC()->PSSetShaderResources(GBUFFERMAP, 6, srvs);

	auto temp = diffuseRTV->SRV();
	D3D::GetDC()->PSSetShaderResources(7, 1, &temp);
	D3D::GetDC()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);

	RenderDirectional();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

	if (bDrawPointLights)
		RenderPointLights();
	if (bDrawSpotLights)
		RenderSpotLights();
}

void GBuffer::DebugRender(Camera* viewer)
{
	for (int i = 0; i < 6; i++)
	{
		debug2D[i]->Update();
		debug2D[i]->PostRender(viewer);
	}
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
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));

	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;//앞면만 그려짐
	Check(D3D::GetDevice()->CreateRasterizerState(&desc, &debugRSS));

	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;

	Check(D3D::GetDevice()->CreateRasterizerState(&desc, &lightRSS));
}

void GBuffer::RenderDirectional()
{
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	noDepthWriteLessDSS->SetState();
	gbufferShader->Render();

	D3D::GetDC()->Draw(4,0);
}

void GBuffer::CalcPointLights(UINT count)
{
	/*for (UINT i = 0; i < count; i++)
	{
		Matrix S, T;
		float s = pointLightDesc.PointLight[i].Range;
		Vector3 t = pointLightDesc.PointLight[i].Position;

		D3DXMatrixScaling(&S, s, s, s);
		D3DXMatrixTranslation(&T, t.x, t.y, t.z);
		pointLightDesc.Projection[i] = S * T * Context::Get()->View() * Context::Get()->Projection();
	}*/
}

void GBuffer::RenderPointLights()
{
	//sPointLightBuffer->SetConstantBuffer(pointLightBuffer->Buffer());
	////PointLightDebug
	//if (bDebug)
	//{
	//	ImGui::InputFloat("PointLight Factor", &pointLightDesc.TessFactor, 1.0f);

	//	sRSS->SetRasterizerState(0, debugRSS);
	//	UINT count = Context::Get()->PointLights(pointLightDesc.PointLight);
	//	CalcPointLights(count);

	//	pointLightBuffer->Apply();

	//	shader->Draw(0, 7, count * 2);//count*2 = SV_PrimitiveID

	//}

	////PointLight
	//{
	//	sRSS->SetRasterizerState(0, lightRSS);
	//	sDSS->SetDepthStencilState(0, noDepthWriteGreaterDSS);

	//	UINT count = Context::Get()->PointLights(pointLightDesc.PointLight);
	//	CalcPointLights(count);
	//	pointLightBuffer->Apply();

	//	shader->Draw(0, 8, count * 2);//count*2 = SV_PrimitiveID
	//}
}

void GBuffer::CalcSpotLights(UINT count)
{
	/*for (UINT i = 0; i < count; i++)
	{
		float angle = Math::ToRadian(spotLightDesc.SpotLight[i].Angle);

		spotLightDesc.Angle[i].x = cosf(angle);
		spotLightDesc.Angle[i].y = sinf(angle);

		Matrix S, R, T;
		float s = spotLightDesc.SpotLight[i].Range;
		Vector3 t = spotLightDesc.SpotLight[i].Position;

		D3DXMatrixScaling(&S, s, s, s);
		D3DXMatrixTranslation(&T, t.x, t.y, t.z);

		Vector3 direction = spotLightDesc.SpotLight[i].Direction;
		bool bUp = (direction.y > 1 - 1e-6f || direction.y < -1 + 1e-6f);
		Vector3 up = bUp ? Vector3(0, 0, direction.y) : Vector3(0, 1, 0);

		Vector3 right;
		D3DXVec3Cross(&right, &up, &direction);
		D3DXVec3Normalize(&right, &right);
		D3DXVec3Cross(&up, &direction, &right);
		D3DXVec3Normalize(&up, &up);

		D3DXMatrixIdentity(&R);
		for (int k = 0; k < 3; k++)
		{
			R.m[0][k] = right[k];
			R.m[1][k] = up[k];
			R.m[2][k] = direction[k];
		}

		spotLightDesc.Projection[i] = S * R * T * Context::Get()->View() * Context::Get()->Projection();
	}*/
}

void GBuffer::RenderSpotLights()
{
	//sSpotLightBuffer->SetConstantBuffer(spotLightBuffer->Buffer());
	////SpotLightDebug
	//if (bDebug)
	//{
	//	ImGui::InputFloat("SpotLight Factor", &spotLightDesc.TessFactor, 1.0f);

	//	sRSS->SetRasterizerState(0, debugRSS);
	//	UINT count = Context::Get()->SpotLights(spotLightDesc.SpotLight);
	//	CalcSpotLights(count);
	//	spotLightBuffer->Apply();

	//	shader->Draw(0, 9, count);//count*2 = SV_PrimitiveID
	//}

	////SpotLight
	//{
	//	sRSS->SetRasterizerState(0, lightRSS);
	//	sDSS->SetDepthStencilState(0, noDepthWriteGreaterDSS);

	//	UINT count = Context::Get()->SpotLights(spotLightDesc.SpotLight);
	//	CalcSpotLights(count);
	//	spotLightBuffer->Apply();

	//	shader->Draw(0, 10, count);//count*2 = SV_PrimitiveID
	//}
}
