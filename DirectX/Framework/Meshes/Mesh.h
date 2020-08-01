#pragma once

#define MAX_MESH_INSTANCE 500

class Mesh : public RenderingNode
{
public:
	typedef VertexTextureNormalTangent MeshVertex;

public:
	Mesh();
	virtual ~Mesh();

	bool CreateBuffer();

	void SetMaterial(wstring diffuseMap, wstring specularMap, wstring normalMap);
	Material* GetMaterial() {
		return material;
	}
	void Update() override;
	void ResourceBinding(Camera* viewer) override;
	void Render(Camera* viewer) override;
public:
	Node* CreateInstance();

protected:
	virtual void CreateMesh() = 0;
protected:
	MeshVertex* vertices;
	UINT* indices;

	Material* material;

private:
	void IncreaseInstancing(Node* object);
	void DecreaseInstancing(Node* object);
	void InitInstanceObject();
	void ReleaseHostObject();
	
	UINT instancingCount;

	VertexBuffer* instancingBuffer;
	Matrix worlds[MAX_MESH_INSTANCE];

	std::map<Node*, Matrix> instanceMatrixList;
};
