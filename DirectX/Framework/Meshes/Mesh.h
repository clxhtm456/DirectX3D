#pragma once

class Mesh : public RenderingNode
{
public:
	typedef VertexTextureNormalTangent MeshVertex;

public:
	Mesh();
	~Mesh();

	bool Init() override;
	void Update();
	void Render();

protected:
	virtual void CreateMesh() = 0;
	
protected:
	Shader * shader;

	VertexBuffer* vertexBuffer = NULL;
	IndexBuffer* indexBuffer = NULL;

	MeshVertex* vertices;
	UINT* indices;

	UINT vertexCount, indexCount;

	Texture* texture;
};
