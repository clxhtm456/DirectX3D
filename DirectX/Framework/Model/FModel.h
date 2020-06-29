#pragma once



class FModel : public RenderingNode
{
public:
	static FModel* Create(string modelDir);
	bool Init(string modelDir);
public:
	FModel();
	~FModel();
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