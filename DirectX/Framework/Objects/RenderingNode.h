#pragma once

class RenderingNode : public Node
{
public:
	RenderingNode();
	virtual ~RenderingNode();
public:
	// Node을(를) 통해 상속됨
	virtual void PostUpdate() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void PreRender() override;
	virtual void PostRender() override;
	virtual void RemoveFromParent() override;
protected:
	Matrix GetWorld();
private:
	WorldBuffer* worldBuffer;
	Matrix _world;
};