#pragma once



class Shader
{
private:
	wstring shaderFile;
	string vsName;
	string psName;
	string gsName;

	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11GeometryShader* geometryShader = nullptr;
	ID3D11ComputeShader* computeShader = nullptr;

	ID3D11InputLayout* inputLayout;

	ID3DBlob* errorBlob;
	ID3DBlob* vertexBlob;
	ID3DBlob* pixelBlob;
	ID3DBlob* geometryBlob;

	ID3D11ShaderReflection* reflection;

	static map<wstring, Shader*> totalShader;

	Shader(wstring shaderFile, string vsName , string psName );
	~Shader();
public:
	static Shader* Add(wstring shaderFile, string vsName = "VS", string psName = "PS");
	static Shader* PSAdd(wstring shaderFile, string psName = "PS");
	static Shader* VSAdd(wstring shaderFile, string vsName = "VS");
	static Shader* CSAdd(wstring shaderFile, string csName = "CS");
	static void Delete();

	void Binding();

	void BindingPS();
	void BindingVS();
	void BindingCS();

	void RecompileVS(string vs);
	void RecompilePS(string ps);

	wstring GetShaderfile();

	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
	void CreateVertexShader();
	void CreatePixelShader();
	void CreateComputeShader(wstring csName);
	void CreateGeometryShader(string gsName = "GS");

	void CreateInputLayout();
};