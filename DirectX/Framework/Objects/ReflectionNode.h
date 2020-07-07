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
	virtual void Render(Camera* viewer) override;
	virtual void PostRender(Camera* viewer) override;
public:
	Shader* GetDepthShader() { return depthShader; }
	ID3D11ShaderResourceView* GetDepthSRV();
	ID3D11ShaderResourceView* GetRenderTargetSRV();
protected:
	Shader* depthShader;//DepthStencilShader

	class DepthStencil* depthStencil;
	class Viewport* viewport;

	//Shader ����
	Shader* vsShaderSlot;
	Shader* psShaderSlot;

	//�׽�Ʈ�� ������Ÿ��
	class RenderTarget* renderTarget;

	bool isDeffered;

};