#include "Framework.h"
#include "Shaderfx.h"

#include <algorithm>

Shaderfx::Shaderfx(wstring file)
	: file(L"../_Shaders/" + file)
{
	initialStateBlock = new StateBlock();
	{
		D3D::GetDC()->RSGetState(&initialStateBlock->RSRasterizerState);
		D3D::GetDC()->OMGetBlendState(&initialStateBlock->OMBlendState, initialStateBlock->OMBlendFactor, &initialStateBlock->OMSampleMask);
		D3D::GetDC()->OMGetDepthStencilState(&initialStateBlock->OMDepthStencilState, &initialStateBlock->OMStencilRef);
	}

	CreateEffect();
}

Shaderfx::~Shaderfx()
{
	for (Technique& temp : techniques)
	{
		for (Pass& pass : temp.Passes)
		{
			SafeRelease(pass.InputLayout);
		}
	}

	SafeDelete(initialStateBlock);
	SafeRelease(effect);
}

void Shaderfx::CreateEffect()
{
	if (Path::ExistFile(file) == false)
	{
		MessageBox(NULL, file.c_str(), L"파일을 찾을 수 없음", MB_OK);
		assert(false);
	}

	ID3DBlob* fxBlob;
	if (Path::GetExtension(file) == L"fx")
	{
		ID3DBlob* error;
		INT flag = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

		HRESULT hr = D3DCompileFromFile(file.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, NULL, "fx_5_0", flag, NULL, &fxBlob, &error);
		if (FAILED(hr))
		{
			if (error != NULL)
			{
				string str = (const char *)error->GetBufferPointer();
				MessageBoxA(NULL, str.c_str(), "Shaderfx Error", MB_OK);
			}
			assert(false);
		}
	}else if (Path::GetExtension(file) == L"hlsl")
	{
		/*UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

		D3DX11CompileFromFile(file.c_str(), nullptr, nullptr,
			"VS", "vs_5_0", flags, 0, nullptr, &vertexBlob, nullptr, nullptr);

		HRESULT hr = DEVICE->CreateVertexShader(vertexBlob->GetBufferPointer(),
			vertexBlob->GetBufferSize(), nullptr, &vertexShader);
		DEBUG(hr);*/
	}
	else if (Path::GetExtension(file) == L"fxo")
	{
		HANDLE fileHandle = CreateFile(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		bool bChecked = fileHandle != INVALID_HANDLE_VALUE;
		assert(bChecked);



		DWORD dataSize = GetFileSize(fileHandle, NULL);
		assert(dataSize != 0xFFFFFFFF);

		void* data = malloc(dataSize);
		DWORD readSize;
		Check(ReadFile(fileHandle, data, dataSize, &readSize, NULL));

		CloseHandle(fileHandle);
		fileHandle = NULL;

		D3DCreateBlob(dataSize, &fxBlob);
		memcpy(fxBlob->GetBufferPointer(), data, dataSize);

		free(data);
	}
	else
	{
		wstring errorMsg = wstring(L"이펙트 파일이 아님 : ") + file;
		MessageBox(NULL, errorMsg.c_str(), L"Shaderfx Error", MB_OK);

		assert(false);
	}
	Check(D3DX11CreateEffectFromMemory(fxBlob->GetBufferPointer(), fxBlob->GetBufferSize(), 0, D3D::GetDevice(), &effect));


	effect->GetDesc(&effectDesc);
	for (UINT t = 0; t < effectDesc.Techniques; t++)
	{
		Technique technique;
		technique.ITechnique = effect->GetTechniqueByIndex(t);
		technique.ITechnique->GetDesc(&technique.Desc);
		technique.Name = String::ToWString(technique.Desc.Name);

		for (UINT p = 0; p < technique.Desc.Passes; p++)
		{
			Pass pass;
			pass.IPass = technique.ITechnique->GetPassByIndex(p);
			pass.IPass->GetDesc(&pass.Desc);
			pass.Name = String::ToWString(pass.Desc.Name);
			pass.IPass->GetVertexShaderDesc(&pass.PassVsDesc);
			pass.PassVsDesc.pShaderVariable->GetShaderDesc(pass.PassVsDesc.ShaderIndex, &pass.EffectVsDesc);

			for (UINT s = 0; s < pass.EffectVsDesc.NumInputSignatureEntries; s++)
			{
				D3D11_SIGNATURE_PARAMETER_DESC desc;

				HRESULT hr = pass.PassVsDesc.pShaderVariable->GetInputSignatureElementDesc(pass.PassVsDesc.ShaderIndex, s, &desc);
				Check(hr);

				pass.SignatureDescs.push_back(desc);
			}

			pass.InputLayout = CreateInputLayout(fxBlob, &pass.EffectVsDesc, pass.SignatureDescs);
			pass.StateBlock = initialStateBlock;

			technique.Passes.push_back(pass);
		}

		techniques.push_back(technique);
	}

	for (UINT i = 0; i < effectDesc.ConstantBuffers; i++)
	{
		ID3DX11EffectConstantBuffer* iBuffer;
		iBuffer = effect->GetConstantBufferByIndex(i);

		D3DX11_EFFECT_VARIABLE_DESC vDesc;
		iBuffer->GetDesc(&vDesc);

		int a = 0;
	}

	for (UINT i = 0; i < effectDesc.GlobalVariables; i++)
	{
		ID3DX11EffectVariable* iVariable;
		iVariable = effect->GetVariableByIndex(i);

		D3DX11_EFFECT_VARIABLE_DESC vDesc;
		iVariable->GetDesc(&vDesc);

		int a = 0;
	}

	SafeRelease(fxBlob);
}

ID3D11InputLayout * Shaderfx::CreateInputLayout(ID3DBlob * fxBlob, D3DX11_EFFECT_SHADER_DESC* effectVsDesc, vector<D3D11_SIGNATURE_PARAMETER_DESC>& params)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (D3D11_SIGNATURE_PARAMETER_DESC& paramDesc : params)
	{
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		string name = paramDesc.SemanticName;
		transform(name.begin(), name.end(), name.begin(), toupper);

		if (name == "POSITION")
		{
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			//elementDesc.InputSlot = paramDesc.SemanticIndex;
		}


		if (String::StartsWith(name, "INST") == true)
		{
			elementDesc.InputSlot = 1;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			elementDesc.InstanceDataStepRate = 1;
		}

		if (String::StartsWith(name, "SV_") == false)
			inputLayoutDesc.push_back(elementDesc);
	}


	const void* pCode = effectVsDesc->pBytecode;
	UINT pCodeSize = effectVsDesc->BytecodeLength;

	if (inputLayoutDesc.size() > 0)
	{
		ID3D11InputLayout* inputLayout = NULL;
		HRESULT hr = D3D::GetDevice()->CreateInputLayout
		(
			&inputLayoutDesc[0]
			, inputLayoutDesc.size()
			, pCode
			, pCodeSize
			, &inputLayout
		);
		Check(hr);

		return inputLayout;
	}

	return NULL;
}

void Shaderfx::Pass::Draw(UINT vertexCount, UINT startVertexLocation)
{
	BeginDraw();
	{
		D3D::GetDC()->Draw(vertexCount, startVertexLocation);
	}
	EndDraw();
}

void Shaderfx::Pass::DrawIndexed(UINT indexCount, UINT startIndexLocation, INT baseVertexLocation)
{
	BeginDraw();
	{
		D3D::GetDC()->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
	}
	EndDraw();
}

void Shaderfx::Pass::DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation)
{
	BeginDraw();
	{
		D3D::GetDC()->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
	}
	EndDraw();
}

void Shaderfx::Pass::DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation)
{
	BeginDraw();
	{
		D3D::GetDC()->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startIndexLocation);
	}
	EndDraw();
}

void Shaderfx::Pass::BeginDraw()
{
	IPass->ComputeStateBlockMask(&StateBlockMask);

	D3D::GetDC()->IASetInputLayout(InputLayout);
	IPass->Apply(0, D3D::GetDC());
}

void Shaderfx::Pass::EndDraw()
{
	if (StateBlockMask.RSRasterizerState == 1)
		D3D::GetDC()->RSSetState(StateBlock->RSRasterizerState);

	if (StateBlockMask.OMDepthStencilState == 1)
		D3D::GetDC()->OMSetDepthStencilState(StateBlock->OMDepthStencilState, StateBlock->OMStencilRef);

	if (StateBlockMask.OMBlendState == 1)
		D3D::GetDC()->OMSetBlendState(StateBlock->OMBlendState, StateBlock->OMBlendFactor, StateBlock->OMSampleMask);

	D3D::GetDC()->HSSetShader(NULL, NULL, 0);
	D3D::GetDC()->DSSetShader(NULL, NULL, 0);
	D3D::GetDC()->GSSetShader(NULL, NULL, 0);
}

void Shaderfx::Pass::Dispatch(UINT x, UINT y, UINT z)
{
	IPass->Apply(0, D3D::GetDC());
	D3D::GetDC()->Dispatch(x, y, z);


	ID3D11ShaderResourceView* null[1] = { 0 };
	D3D::GetDC()->CSSetShaderResources(0, 1, null);

	ID3D11UnorderedAccessView* nullUav[1] = { 0 };
	D3D::GetDC()->CSSetUnorderedAccessViews(0, 1, nullUav, NULL);

	D3D::GetDC()->CSSetShader(NULL, NULL, 0);
}

void Shaderfx::Technique::Draw(UINT pass, UINT vertexCount, UINT startVertexLocation)
{
	Passes[pass].Draw(vertexCount, startVertexLocation);
}

void Shaderfx::Technique::DrawIndexed(UINT pass, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation)
{
	Passes[pass].DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void Shaderfx::Technique::DrawInstanced(UINT pass, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation)
{
	Passes[pass].DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void Shaderfx::Technique::DrawIndexedInstanced(UINT pass, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation)
{
	Passes[pass].DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void Shaderfx::Technique::Dispatch(UINT pass, UINT x, UINT y, UINT z)
{
	Passes[pass].Dispatch(x, y, z);
}

void Shaderfx::Draw(UINT technique, UINT pass, UINT vertexCount, UINT startVertexLocation)
{
	techniques[technique].Passes[pass].Draw(vertexCount, startVertexLocation);
}

void Shaderfx::DrawIndexed(UINT technique, UINT pass, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation)
{
	techniques[technique].Passes[pass].DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void Shaderfx::DrawInstanced(UINT technique, UINT pass, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation)
{
	techniques[technique].Passes[pass].DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void Shaderfx::DrawIndexedInstanced(UINT technique, UINT pass, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation)
{
	techniques[technique].Passes[pass].DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void Shaderfx::Dispatch(UINT technique, UINT pass, UINT x, UINT y, UINT z)
{
	techniques[technique].Passes[pass].Dispatch(x, y, z);
}

ID3DX11EffectVariable * Shaderfx::Variable(string name)
{
	return effect->GetVariableByName(name.c_str());
}

ID3DX11EffectScalarVariable * Shaderfx::AsScalar(string name)
{
	return effect->GetVariableByName(name.c_str())->AsScalar();
}

ID3DX11EffectVectorVariable * Shaderfx::AsVector(string name)
{
	return effect->GetVariableByName(name.c_str())->AsVector();
}

ID3DX11EffectMatrixVariable * Shaderfx::AsMatrix(string name)
{
	return effect->GetVariableByName(name.c_str())->AsMatrix();
}

ID3DX11EffectStringVariable * Shaderfx::AsString(string name)
{
	return effect->GetVariableByName(name.c_str())->AsString();
}

ID3DX11EffectShaderResourceVariable * Shaderfx::AsSRV(string name)
{
	return effect->GetVariableByName(name.c_str())->AsShaderResource();
}

ID3DX11EffectRenderTargetViewVariable * Shaderfx::AsRTV(string name)
{
	return effect->GetVariableByName(name.c_str())->AsRenderTargetView();
}

ID3DX11EffectDepthStencilViewVariable * Shaderfx::AsDSV(string name)
{
	return effect->GetVariableByName(name.c_str())->AsDepthStencilView();
}

ID3DX11EffectConstantBuffer * Shaderfx::AsConstantBuffer(string name)
{
	return effect->GetConstantBufferByName(name.c_str());
}

ID3DX11EffectShaderVariable * Shaderfx::AsShader(string name)
{
	return effect->GetVariableByName(name.c_str())->AsShader();
}

ID3DX11EffectBlendVariable * Shaderfx::AsBlend(string name)
{
	return effect->GetVariableByName(name.c_str())->AsBlend();
}

ID3DX11EffectDepthStencilVariable * Shaderfx::AsDepthStencil(string name)
{
	return effect->GetVariableByName(name.c_str())->AsDepthStencil();
}

ID3DX11EffectRasterizerVariable * Shaderfx::AsRasterizer(string name)
{
	return effect->GetVariableByName(name.c_str())->AsRasterizer();
}

ID3DX11EffectSamplerVariable * Shaderfx::AsSampler(string name)
{
	return effect->GetVariableByName(name.c_str())->AsSampler();
}

ID3DX11EffectUnorderedAccessViewVariable * Shaderfx::AsUAV(string name)
{
	return effect->GetVariableByName(name.c_str())->AsUnorderedAccessView();
}