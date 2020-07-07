#pragma once

#define MAX_MESH_INSTANCE 500

class Mesh : public RenderingNode
{
public:
	typedef VertexTextureNormalTangent MeshVertex;

public:
	Mesh();
	~Mesh();

	bool CreateBuffer();

	void SetMaterial(wstring diffuseMap, wstring specularMap, wstring normalMap);
	Material* GetMaterial() {
		return material;
	}
	void Destroy() override;
	void Update() override;
	void Render(Camera* viewer) override;
	void Draw(Camera* viewer) override;
public:
	Node* CreateInstance();

protected:
	virtual void CreateMesh() = 0;
	
protected:
	MeshVertex* vertices;
	UINT* indices;

	Material* material;

	RasterizerState* rasterizerState;
private:
	void IncreaseInstancing(Node* object);
	void DecreaseInstancing(Node* object);
	void UpdateInstancingMatrix();
	UINT instancingCount;

	VertexBuffer* instancingBuffer;
	Matrix worlds[MAX_MESH_INSTANCE];

	std::vector<Node*> instanceNode;
};
