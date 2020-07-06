#pragma once

class InstancingObject :public Node
{
public:
	friend class Mesh;
private:
	static InstancingObject* Create(Mesh* master);
	bool Init(Mesh* master);
private:
	InstancingObject();
	virtual ~InstancingObject();
protected:
	virtual void PostUpdate() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render(Camera* viewer) override;
	virtual void PostRender(Camera* viewer) override;
	void Destroy() override;
private:
	Mesh* master;

};