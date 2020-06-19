#pragma once



class Shader
{
private:
	wstring shaderFile;
	string vsName;
	string psName;
	string gsName;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11GeometryShader* geometryShader;

	ID3D11InputLayout* inputLayout;

	ID3DBlob* errorBlob;
	ID3DBlob* vertexBlob;
	ID3DBlob* pixelBlob;
	ID3DBlob* geometryBlob;

	ID3D11ShaderReflection* reflection;

	static map<wstring, Shader*> totalShader;

	Shader(wstring shaderFile, string vsName = "VS", string psName = "PS");
	~Shader();
public:
	static Shader* Add(wstring shaderFile, string vsName = "VS", string psName = "PS");
	static void Delete();

	void Render();

	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
	void CreateVertexShader();
	void CreatePixelShader();
	void CreateGeometryShader(string gsName = "GS");

	void CreateInputLayout();
};