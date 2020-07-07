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

	virtual void SetShader(wstring file, string vs = "VS", string ps = "PS");
	virtual void SetPSShader(wstring file, string ps = "PS");
	virtual void SetVSShader(wstring file, string vs = "VS");
	virtual void SetShader(Shader* nShader);
	virtual void SetPSShader(Shader* nShader);
	virtual void SetVSShader(Shader* nShader);

	Shader* GetVSShader();
	Shader* GetPSShader();

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

};