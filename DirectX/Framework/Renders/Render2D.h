#pragma once

class Render2D : public RenderingNode
{
public:
	static Render2D* Create();
	bool Init();
public:
	Render2D();
	virtual ~Render2D();

	void Update() override;
	void Render(Camera* viewer) override;

	virtual void SRV(ID3D11ShaderResourceView* srv);
private:
	struct ViewProjectionDesc
	{
		Matrix View;
		Matrix Projection;
	} vpDesc;

protected:
	UINT pass;
	Shader* shader;

	Vector2 textureSize;

private:
	ConstantBuffer* vpBuffer;
	
	VertexBuffer* vertexBuffer;

	ID3D11ShaderResourceView* diffuseMap = nullptr;
	ID3D11SamplerState* diffuseSampler = nullptr;
};