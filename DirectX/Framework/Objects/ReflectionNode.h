#pragma once

class ReflectionNode : public Node
{
public :
	ReflectionNode(bool isDeffered = false);
	virtual ~ReflectionNode();
public:
	virtual UINT ChildType()
	{
		return TYPE_REFLECTION;
	}
	virtual void SetUpRender() = 0;
	virtual void SetRNShader2Depth(RenderingNode* node);
	virtual void SetRNShader2Origin(RenderingNode* node);
protected:
	virtual void PostUpdate() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void ResourceBinding(Camera* viewer) override;
	virtual void PostRender(Camera* viewer) override;
public:
	Shader* GetDepthShader() { return depthShader; }
	class DepthStencil* GetDepth();
	class RenderTarget* GetRenderTarget();
protected:
	Shader* depthShader;//DepthStencilShader

	class DepthStencil* depthStencil;
	class Viewport* viewport;

	//Shader 저장
	Shader* vsShaderSlot;
	Shader* psShaderSlot;

	//테스트용 렌더링타겟
	class RenderTarget* renderTarget;

	bool isDeffered;

};