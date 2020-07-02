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
	void Draw(Camera* viewer) override;

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
	
	VertexBuffer* vertexBuffer;

	ID3D11ShaderResourceView* diffuseMap = NULL;
	ID3D11SamplerState* diffuseSampler = NULL;

	DepthStencilState* depthState[2];
};