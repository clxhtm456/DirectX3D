#pragma once

class RenderingNode : public Node
{
public:
	RenderingNode();
	virtual ~RenderingNode();
public:
	// Node��(��) ���� ��ӵ�
	virtual bool Init() override;
	virtual void PostUpdate() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void PreRender() override;
	virtual void PostRender() override;
	virtual void RemoveFromParent() override;
private:
	WorldBuffer* worldBuffer;
};