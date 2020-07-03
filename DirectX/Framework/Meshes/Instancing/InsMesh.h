#pragma once

class InsMesh : public RenderingNode
{
public:
	typedef VertexTextureNormalTangent MeshVertex;

public:
	InsMesh();
	~InsMesh();

	bool CreateBuffer();

	void SetMaterial(wstring diffuseMap, wstring specularMap, wstring normalMap);
	Material* GetMaterial() {
		return material;
	}

	void Update() override;
	void Render(Camera* viewer) override;

protected:
	virtual void CreateMesh() = 0;
	
protected:
	MeshVertex* vertices;
	UINT* indices;

	Material* material;

	RasterizerState* rasterizerState;
private:
	static UINT instancingIndex;
};
