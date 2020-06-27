#pragma once


class DirectionLight : public Light
{
public:
	static DirectionLight* Create();
	bool Init();

private:
	DirectionLight();
	~DirectionLight();

public:
	void SetBuffer(OUT class LightBuffer* buffer);

private:
	Vector3 direction = Vector3(-1, -1, 1);
	float specExp = 8.0f;

	Color ambient = Color(0.1f, 0.1f, 0.1f, 1.0f);

	int isSpecularMap = 0;
	int isNormalMap = 0;

protected:
	// Node을(를) 통해 상속됨
	void PostUpdate() override;
	void Update() override;
	void LateUpdate() override;
	void Render(Camera* viewer) override;
	void PreRender(Camera* viewer) override;
	void PostRender(Camera* viewer) override;
	void RemoveFromParent() override;
};