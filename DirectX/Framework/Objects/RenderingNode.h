#pragma once

class RenderingNode : public Node
{
public:
	RenderingNode();
	virtual ~RenderingNode();
public:
	// Node을(를) 통해 상속됨
	virtual void Start() override;
	virtual void PostUpdate() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render(Camera* viewer) override;
	virtual void PostRender(Camera* viewer) override;
	virtual void Draw(Camera* viewer) override;
public:
	void SetPosition(Vector3 position)override;
	void SetRotation(Vector3 rotation)override;
	void SetRotationDegree(Vector3 rotation)override;
	void SetScale(Vector3 scale)override;

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetRotationDegree(float x, float y, float z);
	void SetScale(float x, float y, float z);

	void SetShader(wstring file, string vs = "VS", string ps = "PS");
	void SetPSShader(wstring file, string ps = "PS");
	void SetVSShader(wstring file, string vs = "VS");
	void SetShader(Shader* nShader);
	void SetPSShader(Shader* nShader);
	void SetVSShader(Shader* nShader);
	void UseShadow(bool val) { _useShadow = val; }
	bool GetUseShadow() { return _useShadow; }
	void SetShadowMap(ID3D11ShaderResourceView* srv) { shadowMap = srv; }

	Shader* GetVSShader();
	Shader* GetPSShader();
	RasterizerState* GetRasterizerState();

	void WorldSet();
	void VPSet(Camera* viewer);
	void LightSet();

	virtual void CalcWorldMatrix();
protected:
	Shader* shader = NULL;
	Shader* psShader = NULL;
	Shader* vsShader = NULL;
	VertexBuffer* vertexBuffer = NULL;
	IndexBuffer* indexBuffer = NULL;

	UINT vertexCount = 0;
	UINT indexCount = 0;

	WorldBuffer* worldBuffer;
	LightBuffer* lightBuffer;

	RasterizerState* rasterizerState;

	ID3D11ShaderResourceView* shadowMap = NULL;
private:
	bool _useShadow = true;

};