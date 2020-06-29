#pragma once



class Model : public RenderingNode
{
public:
	static Model* Create(string modelDir);
	bool Init(string modelDir);
public:
	Model();
	~Model();
public:
	void CalcWorldMatrix() override;
	void Update() override;
	void Render(Camera* viewer) override;
private:
	class ModelData* modelData;
	vector<Matrix> boneTransforms;
	IntBuffer* intBuffer;

	RasterizerState* rstate;

};