#pragma once

class Mesh : public RenderingNode
{
public:
	typedef VertexTextureNormalTangent MeshVertex;

public:
	Mesh();
	~Mesh();

	bool CreateBuffer();

	void SetMaterial(wstring diffuseMap, wstring specularMap, wstring normalMap);

	void Update() override;
	void Render(Camera* viewer) override;

protected:
	virtual void CreateMesh() = 0;
	
protected:
	VertexBuffer* vertexBuffer = NULL;
	IndexBuffer* indexBuffer = NULL;

	MeshVertex* vertices;
	UINT* indices;

	UINT vertexCount, indexCount;

	Material* material;

	RasterizerState* rasterizerState;
};
