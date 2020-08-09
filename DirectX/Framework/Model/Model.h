#pragma once

#define MAX_MODEL_KEY	500
#define MAX_MODEL_INSTANCE	500

class Model : public RenderingNode
{
private:
    struct BoneNode
    {
		int index;
		string name;
        XMFLOAT3 translate = { 0.f,0.f,0.f };
        XMFLOAT4 preQuaternion = { 0.f,0.f,0.f,1.f };
        XMFLOAT4 quaternion = { 0.f,0.f,0.f,1.f };
        XMFLOAT3 scale = { 1.f,1.f,1.f };
        int parentID = -1;
        Matrix local;
		Matrix world;
		Matrix offset;
		BoneNode()
        {
			local = XMMatrixIdentity();
			world = XMMatrixIdentity();
			offset = XMMatrixIdentity();
        }
    };

    struct VertexType
    {
        XMFLOAT3 position = { 0.f,0.f,0.f };
        XMFLOAT2 uv = { 0.f,0.f };
        XMFLOAT3 normal = { 0.f,0.f,0.f };
        XMFLOAT3 tangent = { 0.f,0.f,0.f };
        XMUINT4 boneid = { 0,0,0,0 };
        XMFLOAT4 weight = { 0.f,0.f,0.f,0.f };
    };

    struct Mesh
    {
        UINT matrialID = 0;
        UINT indexCount = 0;
        VertexBuffer* vertexBuffer = nullptr;
        IndexBuffer* indexBuffer = nullptr;
		ID3D11Buffer* meshBuffer = nullptr;//메쉬 하이어라키 위치버퍼
    };

    struct MaterialData
    {
        XMFLOAT3 diffuse = { 0.f,0.f,0.f };
        FLOAT bumpscaling = 0.f;
        XMFLOAT3 ambient = { 0.f,0.f,0.f };
        FLOAT opacity = 0.f;
        XMFLOAT3 specular = { 0.f,0.f,0.f };
        FLOAT shininess = 0.f;
        XMFLOAT3 emissive = { 0.f,0.f,0.f };
        FLOAT shininessstrength = 0.f;
        XMFLOAT3 tranparent = { 1.f,1.f,1.f };
        FLOAT transparentfactor = 0.f;
        XMFLOAT3 reflective = { 0.f,0.f,0.f };
        FLOAT reflectivity = 0.f;

        FLOAT refracti = 0.f;
        INT opaque = 1;
        INT reflector = 0;
        INT hasDiffuseMap = 0;

        INT hasSpecularMap = 0;
        INT hasAmbientMap = 0;
        INT hasEmissiveMap = 0;
        INT hasHeightMap = 0;

        INT hasNormalMap = 0;
        INT hasShininessMap = 0;
        INT hasOpacityMap = 0;
        INT hasDisplacementMap = 0;

        INT hasLightMapMap = 0;
        INT hasReflectionMap = 0;
        INT hasBasecolorMap = 0;
        INT hasNormalcameraMap = 0;

        INT hasEmissioncolorMap = 0;
        INT hasMetalnessMap = 0;
        INT hasDiffuseroughnessMap = 0;
        INT hasAmbientocculsionMap = 0;

        //TODO : 메터리얼 변수값 나중에 셰이더마다 필요한것만 네이밍 규칙정해서 일괄로 처리하자
    };


	struct ClipTransform
	{
		Matrix** transform;

		ClipTransform()
		{
			transform = new Matrix * [MAX_MODEL_KEY];

			for (UINT i = 0; i < MAX_MODEL_KEY; i++)
				transform[i] = new Matrix[MAX_MODEL_BONE];
		}

		~ClipTransform()
		{
			for (UINT i = 0; i < MAX_MODEL_KEY; i++)
			{
				delete[] transform[i];
			}
			delete[] transform;
		}
	};

	struct KeyFrameDesc
	{
		int clip;

		UINT curFrame, nextFrame;

		float time, runningTime;

		float speed;

		float padding[2];

		KeyFrameDesc()
		{
			clip = 0;
			curFrame = 0;
			nextFrame = 0;

			time = 0.0f;
			runningTime = 0.0f;

			speed = 1.0f;
		}
	};

	struct TweenDesc
	{
		float takeTime;
		float tweenTime;
		float runningTime;
		float padding;

		KeyFrameDesc cur;
		KeyFrameDesc next;

		TweenDesc()
		{
			takeTime = 1.0f;
			tweenTime = 0.0f;
			runningTime = 0.0f;

			cur.clip = 0;
			next.clip = -1;
		}
	};

	class FrameBuffer : public ConstantBuffer
	{
	public:
		struct Data
		{
			TweenDesc tweenDesc[MAX_MODEL_INSTANCE];
		}data;

		FrameBuffer() : ConstantBuffer(&data, sizeof(Data))
		{
		}
	};

//    struct ANIMATIONCTRL
//    {
//        float speed = 1.f;
//        float fadeIn = 0.f;
//        float fadeOut = 0.f;
//    };
//
//    struct ANIMATION
//    {
//        int clip = 0;
//        float curtime = 0.f;
//        float duration = 1.f;
//        float blendLoop = 0.f;
//    };
//
//    struct INSTANCEANIMATION
//    {
//        ANIMATION cur;
//        ANIMATION next;
//
//        float blendFactor = 0.f;
//    };
//public:
//	struct KEYFRAME
//	{
//		float tickPerSec = 0.f;
//		float duration = 0.f;
//		float blendLoop = 0.f;
//		bool isLoop = true;
//		vector< vector<XMFLOAT3X4>> keyframes;
//		/*
//		*Translate,
//		*Quaternion
//		*Scale
//		*/
//
//		function<void()> EndEvent;
//	};

public:
	static Model* Create(string path);
	bool Init(string path);

public:
    Model();
    ~Model();

public:
    void Update()override;
	void ResourceBinding(Camera* viewer) override;
    void Render(Camera* viewer)override;

    void LoadAnimation(const string path, float blendLoop = 1.f, bool isLoop = true);

	void CopyGlobalBoneTo(vector<Matrix>& transforms);
	void CopyGlobalBoneTo(vector<Matrix>& transforms, Matrix& w);

    void SetEndEvent(int index, function<void()> Event) { }
    void SetNextAction(int index, function<void(int)> Event) { }

    void SetTexture(string name, string attr, string texPath);

    ID3D11ShaderResourceView* GetTexture(string name) {
        if (materialMap.count(name))
            return materials[materialMap[name]]->GetDiffuseMap();
        return nullptr;
    }
public:
	Node* CreateInstance();
private:
	void InitInstanceObject();
	void IncreaseInstancing(Node* object);
	void DecreaseInstancing(Node* object);
	UINT instancingCount;

	VertexBuffer* instancingBuffer;
	Matrix worlds[MAX_MESH_INSTANCE];

	std::map<Node*, Matrix> instanceMatrixList;
private:
    void LoadModel(const string path);

	UINT nodeCount;
    //메쉬 랜더링 파트
    vector<Model::Mesh> meshs;
    vector<Material*> materials;
    map<string, UINT> materialMap;
    UINT stride = sizeof(VertexType);
    UINT offset = 0;

	vector<BoneNode*> bones;

/*
*Animation
*/
public:
	void AnimUpdate();

	void Play(int clipIndex, bool isRepeat = false, float blendTime = 10.0f,
		float speed = 15.0f, float startTime = 0.0f);
protected:

	vector<Matrix> boneTransforms;
	vector<class ModelClip*> clips;//불러들인 전체 애니메이션 저장용 벡터
	BoneBuffer* boneBuffer;

private:
	int nextAnim = -1;
	int currentAnim = -1;

	float blendTime = 0.0f;
	float elapsedTime = 0.0f;

	float frameCount = 0;
	bool isLockRoot = true;


};
