#pragma once

class Render2D : public Node
{
public:
	static Render2D* Create();
	bool Init();
public:
	Render2D();
	virtual ~Render2D();

	void Update() override;
	void ResourceBinding(Camera* viewer) override;
	void Render(Camera* viewer) override;

	void PostUpdate() override;
	void LateUpdate() override;
	void PostRender(Camera* viewer) override;

	virtual void SetSRV(ID3D11ShaderResourceView* srv);
private:
	struct ViewProjectionDesc
	{
		Matrix View;
		Matrix Projection;
	} vpDesc;

protected:
	Vector2 textureSize;
private:
	ConstantBuffer* vpBuffer;

	ID3D11ShaderResourceView* diffuseMap = NULL;
	ID3D11SamplerState* diffuseSampler = NULL;

	DepthStencilState* depthState[2];

	Shader* shader = NULL;

	VertexBuffer* vertexBuffer = NULL;

	WorldBuffer* worldBuffer;
private:
	Matrix _world;

	// Node을(를) 통해 상속됨
	
};