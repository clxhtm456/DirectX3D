#include "Framework.h"
#include "SkyCube.h"

SkyCube::SkyCube(wstring file,Shader* shader):
	shader(shader)
{
	if (shader == nullptr)
	{
		shader = new Shader(L"14_SkyCube.fx");
		bCreateShader = true;
	}
	
	sphereRender = new MeshRender(this->shader, new MeshSphere(0.5f));
	sphereRender->AddTransform()->Scale(500,500,500);

	wstring temp = L"../../_Textures/" + file;
	Check(D3DX11CreateShaderResourceViewFromFile
	(	
		D3D::GetDevice(), temp.c_str(), NULL, NULL, &srv, NULL
	));

	sSrv = this->shader->AsSRV("SkyCubeMap");

	//DepthStencilState
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		desc.DepthEnable = false;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = true;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		D3D11_DEPTH_STENCILOP_DESC opDesc;
		//depth 검사시 카메라에 더 가까운 하늘에 의해 오브젝트는 stencil 검사에서 탈락 탈락햇을경우에도 keep하는 옵션
		opDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		opDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		opDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		opDesc.StencilFunc = D3D11_COMPARISON_EQUAL;
		desc.FrontFace = opDesc;
		desc.BackFace = opDesc;

		Check(D3D::GetDevice()->CreateDepthStencilState(&desc, &dss));
	}
	sDss = shader->AsDepthStencil("SkyDepthStencil");
}

SkyCube::~SkyCube()
{
	if(bCreateShader)
		SafeDelete(shader);
	SafeDelete(sphereRender);
	SafeRelease(srv);
	SafeRelease(dss);
}

void SkyCube::Update()
{
	Vector3 position;
	Context::Get()->GetCamera()->Position(&position);

	sphereRender->GetTransform(0)->Position(position);
	sphereRender->UpdateTransforms();
}

void SkyCube::Render()
{	
	sDss->SetDepthStencilState(0, dss);
	sSrv->SetResource(srv);
	
	sphereRender->Pass(pass);
	sphereRender->Render();
}
