#pragma once

class InstancingObject :public Node
{
public:
	friend class Mesh;
private:
	static InstancingObject* Create();
	bool Init();
private:
	InstancingObject();
	virtual ~InstancingObject();
protected:
	virtual void PostUpdate() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render(Camera* viewer) override;
	virtual void PostRender(Camera* viewer) override;
private:

};