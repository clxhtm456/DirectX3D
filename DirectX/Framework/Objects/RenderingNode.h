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
	virtual void PreRender(Camera* viewer) override;
	virtual void PostRender(Camera* viewer) override;
	virtual void RemoveFromParent() override;
public:
	void SetPosition(Vector3 position)override;
	void SetRotation(Vector3 rotation)override;
	void SetRotationDegree(Vector3 rotation)override;
	void SetScale(Vector3 scale)override;

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetRotationDegree(float x, float y, float z);
	void SetScale(float x, float y, float z);

	virtual void SetShader(wstring file);

	void WorldSet();
	void VPSet(Camera* viewer);
	void LightSet();

	virtual void CalcWorldMatrix();
protected:
	Matrix GetWorld();
	Shader* shader;
private:
	WorldBuffer* worldBuffer;
	LightBuffer* lightBuffer;

	Matrix _world;
};