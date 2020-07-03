#pragma once

class Light : public Node
{
protected:
	Light();
	virtual ~Light();
protected:
	// Node��(��) ���� ��ӵ�
	virtual void Start() override;
	virtual void PostUpdate() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render(Camera* viewer) override;
	virtual void PostRender(Camera* viewer) override;

public:
	UINT ChildType() override
	{
		return TYPE_LIGHT;
	}
};