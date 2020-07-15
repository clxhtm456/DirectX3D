#pragma once



class Model : public RenderingNode
{
public:
	static Model* Create(string modelDir);
	bool Init(string modelDir);
protected:
	Model();
	virtual ~Model();
public:
	Node* CreateInstance();
private:
	void StartInstancingMode();
	void IncreaseInstancing(Node* object);
	void DecreaseInstancing(Node* object);
	void UpdateInstancingMatrix();
	UINT instancingCount;

	VertexBuffer* instancingBuffer;
	Matrix worlds[MAX_MESH_INSTANCE];

	std::map<Node*, Matrix> instanceMatrixList;

	bool bInstancingMode;
public:
	void CalcWorldMatrix() override;
	void Update() override;
	void Render(Camera* viewer) override;

	class ModelData* GetModelData() { return modelData; }
public:
	/*Color GetAmbient() { return materialBuffer->data.ambient; }
	void SetAmbient(Color color);
	void SetAmbient(float r, float g, float b, float a = 1.0f);

	Color GetDiffuse() { return materialBuffer->data.diffuse; }
	void SetDiffuse(Color color);
	void SetDiffuse(float r, float g, float b, float a = 1.0f);

	Color GetSpecular() { return materialBuffer->data.specular; }
	void SetSpecular(Color color);
	void SetSpecular(float r, float g, float b, float a = 1.0f);

	Color GetEmissive() { return materialBuffer->data.emissive; }
	void SetEmissive(Color color);
	void SetEmissive(float r, float g, float b, float a = 1.0f);*/
protected:
	class ModelData* modelData;
	vector<Matrix> boneTransforms;
	IntBuffer* intBuffer;


};