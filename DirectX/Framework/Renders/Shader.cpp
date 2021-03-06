#include "Framework.h"

#include "DirectXTex.h"
#include "Shader.h"


map<wstring, Shader*> Shader::totalShader;

Shader::Shader(wstring shaderFile, string vsName, string psName)
	: vsName(vsName), psName(psName), gsName(""),
	vertexShader(nullptr),
	pixelShader(nullptr),
	inputLayout(nullptr),
	vertexBlob(nullptr),
	pixelBlob(nullptr),
	geometryShader(nullptr), 
	geometryBlob(nullptr)
{
	this->shaderFile = L"../../_Shaders/" + shaderFile + L".hlsl";
	
}

Shader::~Shader()
{
	if(vertexShader != nullptr)
		vertexShader->Release();
	if(pixelShader != nullptr)
		pixelShader->Release();
	if(inputLayout != nullptr)
		inputLayout->Release();
	if(vertexBlob != nullptr)
		vertexBlob->Release();
	if(pixelBlob != nullptr)
		pixelBlob->Release();

	if (geometryShader != nullptr)
		geometryShader->Release();
	if (geometryBlob != nullptr)
		geometryBlob->Release();
}

Shader* Shader::Add(wstring shaderFile, string vsName, string psName)
{
	if (totalShader.count(shaderFile) > 0)
		return totalShader[shaderFile];
	else
	{
		Shader* temp = new Shader(shaderFile, vsName, psName);
		totalShader.insert({ shaderFile,  temp });

		if (vsName != "")
		{
			temp->CreateVertexShader();
			temp->CreateInputLayout();
		}
		if (psName != "")
		{
			temp->CreatePixelShader();
		}
		return totalShader[shaderFile];
	}

	
}

Shader* Shader::PSAdd(wstring shaderFile, string psName)
{
	if (totalShader.count(shaderFile) > 0)
		return totalShader[shaderFile];
	else
	{
		Shader* temp = new Shader(shaderFile, "", psName);
		totalShader.insert({ shaderFile, temp });

		if (psName != "")
		{
			temp->CreatePixelShader();
		}

		return totalShader[shaderFile];
	}
}

Shader* Shader::VSAdd(wstring shaderFile, string vsName)
{
	if (totalShader.count(shaderFile) > 0)
		return totalShader[shaderFile];
	else
	{
		Shader* temp = new Shader(shaderFile, vsName, "");
		totalShader.insert({ shaderFile, temp });

		if (vsName != "")
		{
			temp->CreateVertexShader();
			temp->CreateInputLayout();
		}

		return totalShader[shaderFile];
	}
}

Shader* Shader::CSAdd(wstring shaderFile, string csName)
{
	if (totalShader.count(shaderFile) > 0)
		return totalShader[shaderFile];
	else
	{
		Shader* temp = new Shader(shaderFile, "", "");
		totalShader.insert({ shaderFile, temp });

		if (csName != "")
		{
			temp->CreateComputeShader(String::ToWString(csName));
		}

		return totalShader[shaderFile];
	}
}

void Shader::Delete()
{
	for (auto shader : totalShader)
		delete shader.second;
}

void Shader::Binding()
{
	D3D::GetDC()->IASetInputLayout(inputLayout);
	D3D::GetDC()->VSSetShader(vertexShader, nullptr, 0);

	if (geometryShader != nullptr)
		D3D::GetDC()->GSSetShader(geometryShader, nullptr, 0);

	D3D::GetDC()->PSSetShader(pixelShader, nullptr, 0);
}

void Shader::BindingPS()
{
	if (pixelShader != nullptr)
		D3D::GetDC()->PSSetShader(pixelShader, nullptr, 0);
}

void Shader::BindingVS()
{
	if (vertexShader != nullptr)
	{
		D3D::GetDC()->IASetInputLayout(inputLayout);
		D3D::GetDC()->VSSetShader(vertexShader, nullptr, 0);
	}
	
	if (geometryShader != nullptr)
		D3D::GetDC()->GSSetShader(geometryShader, nullptr, 0);
}

void Shader::BindingCS()
{
	if(computeShader != nullptr)
		D3D::GetDC()->CSSetShader(computeShader, nullptr, 0);
}

void Shader::RecompileVS(string vs)
{
	if (vsName == vs)
		return;

	vsName = vs;

	vertexShader->Release();
	inputLayout->Release();
	vertexBlob->Release();

	CreateVertexShader();
	CreateInputLayout();
}

void Shader::RecompilePS(string ps)
{
	if (psName == ps)
		return;

	psName = ps;
	pixelShader->Release();
	pixelBlob->Release();

	CreatePixelShader();
}

wstring Shader::GetShaderfile()
{
	for (auto iter = totalShader.begin(); iter != totalShader.end(); iter++)
	{
		if ((*iter).second == this)
			return (*iter).first;
	}
	return L"";
}

HRESULT Shader::CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif


	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}

void Shader::CreateVertexShader()
{
	CompileShader(shaderFile.c_str(), vsName.c_str(), "vs_5_0", &vertexBlob);

	HRESULT hr = D3D::GetDevice()->CreateVertexShader(vertexBlob->GetBufferPointer(),
		vertexBlob->GetBufferSize(), nullptr, &vertexShader);
	Check(hr);
}

void Shader::CreatePixelShader()
{
	CompileShader(shaderFile.c_str(), psName.c_str(), "ps_5_0", &pixelBlob);

	HRESULT hr = D3D::GetDevice()->CreatePixelShader(pixelBlob->GetBufferPointer(),
		pixelBlob->GetBufferSize(), nullptr, &pixelShader);
	Check(hr);
}

void Shader::CreateComputeShader(wstring csName)
{
	ID3DBlob* csBlob;
	CompileShader(shaderFile.c_str(), String::ToString(csName).c_str(), "cs_5_0", &csBlob);

	HRESULT hr = D3D::GetDevice()->CreateComputeShader(csBlob->GetBufferPointer(),
		csBlob->GetBufferSize(), nullptr, &computeShader);
	Check(hr);
	csBlob->Release();
}

void Shader::CreateGeometryShader(string gsName)
{
	this->gsName = gsName;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr,
		gsName.c_str(), "gs_5_0", flags, 0, &geometryBlob, nullptr);

	HRESULT hr = D3D::GetDevice()->CreateGeometryShader(geometryBlob->GetBufferPointer(),
		geometryBlob->GetBufferSize(), nullptr, &geometryShader);
	Check(hr);
}

void Shader::CreateInputLayout()
{
	HRESULT hr = D3DReflect(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&reflection);
	Check(hr);

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	vector<D3D11_INPUT_ELEMENT_DESC> inputLayouts;

	for (UINT i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		reflection->GetInputParameterDesc(i, &paramDesc);

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

		string temp = paramDesc.SemanticName;
		transform(temp.begin(), temp.end(), temp.begin(), toupper);
		if (temp == "POSITION")
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

		if (temp == "INSTANCE")
		{
			elementDesc.InputSlot = 1;			
			elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			elementDesc.InstanceDataStepRate = 1;
		}

		//if (String::StartsWith(temp, "SV_") == false)
			inputLayouts.push_back(elementDesc);
		
	}

	hr = D3D::GetDevice()->CreateInputLayout(inputLayouts.data(), inputLayouts.size(),
		vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &inputLayout);
	Check(hr);
}
