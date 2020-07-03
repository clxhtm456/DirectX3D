#pragma once


class DirectionLight : public ReflectionNode
{
public:
	static DirectionLight* Create();
	bool Init();

private:
	DirectionLight();
	~DirectionLight();

public:
	void SetBuffer(OUT class LightBuffer* buffer);
	Vector3 GetDirection() { return direction; }
private:
	Vector3 direction = Vector3(-1, -1, 1);
	Color ambient = Color(0, 0, 0, 1);
	Color specular = Color(1, 1, 1, 1);
	Vector3 position = Vector3(0, 0, 0);
private:
	class Shadow* shadow;

protected:
	void SetRNShader2Origin(RenderingNode* node) override;
	// Node��(��) ���� ��ӵ�
	void PostUpdate() override;
	void Update() override;
	void LateUpdate() override;
	void Render(Camera* viewer) override;
	void PostRender(Camera* viewer) override;

	// ReflectionNode��(��) ���� ��ӵ�
	virtual void SetUpRender() override;
};