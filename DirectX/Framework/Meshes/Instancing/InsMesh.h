#pragma once

class InsMesh : public RenderingNode
{
public:
	typedef VertexTextureNormalTangent MeshVertex;

public:
	InsMesh();
	~InsMesh();

	bool CreateBuffer();

	void Update() override;
	void Render(Camera* viewer) override;

protected:
	virtual void CreateMesh() = 0;
	
protected:
	MeshVertex* vertices;
	UINT* indices;
private:
	static UINT instancingIndex;
};
