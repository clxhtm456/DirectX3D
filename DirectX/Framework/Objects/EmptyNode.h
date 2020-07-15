#pragma once

class EmptyNode :public Node
{
public:
	static EmptyNode* Create();
	bool Init();
private:
	EmptyNode();
	virtual ~EmptyNode();
protected:
	virtual void PostUpdate() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void ResourceBinding(Camera* viewer) override;
	virtual void PostRender(Camera* viewer) override;
private:

};