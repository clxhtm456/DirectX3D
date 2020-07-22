#pragma once

class Model : public RenderingNode
{
private:
    struct HierarchyNode
    {
        XMFLOAT3 translate = { 0.f,0.f,0.f };
        XMFLOAT4 preQuaternion = { 0.f,0.f,0.f,1.f };
        XMFLOAT4 quaternion = { 0.f,0.f,0.f,1.f };
        XMFLOAT3 scale = { 1.f,1.f,1.f };
        int parentID = -1;
        Matrix local;
		Matrix world;
		Matrix offset;
        HierarchyNode()
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
        ConstantBuffer* meshBuffer = nullptr;//�޽� ���̾��Ű ��ġ����
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

        //TODO : ���͸��� ������ ���߿� ���̴����� �ʿ��Ѱ͸� ���̹� ��Ģ���ؼ� �ϰ��� ó������
    };


    struct KEYFRAME
    {
        float tickPerSec = 0.f;
        float duration = 0.f;
        float blendLoop = 0.f;
        bool isLoop = true;
        vector< vector<XMFLOAT3X4>> keyframes;
    };

    struct ANIMATIONCTRL
    {
        float speed = 1.f;
        float fadeIn = 0.f;
        float fadeOut = 0.f;
    };

    struct ANIMATION
    {
        int clip = 0;
        float curtime = 0.f;
        float duration = 1.f;
        float blendLoop = 0.f;
    };

    struct INSTANCEANIMATION
    {
        ANIMATION cur;
        ANIMATION next;

        float blendFactor = 0.f;
    };

public:
	static Model* Create(string path);
	bool Init(string path);

public:
    Model();
    ~Model();

public:

    void LoadAnimation(const string path, float blendLoop = 1.f, bool isLoop = true);

    void Update()override;
	void ResourceBinding(Camera* viewer) override;
    void Render(Camera* viewer)override;
	void CalcWorldMatrix() override;
    void PlayAni(UINT instanceID, int index, float fadeIn = 0.f, float  fadeOut = 0.f)
    {
        if (inatanceAnims[instanceID].cur.clip == index || keyFrames.size() <= index)
            return;

        inatanceAnims[instanceID].next = inatanceAnims[instanceID].cur;
        inatanceAnims[instanceID].cur.clip = index;
        inatanceAnims[instanceID].cur.curtime = 0.f;
        inatanceAnims[instanceID].cur.duration = keyFrames[index].duration;
        inatanceAnims[instanceID].cur.blendLoop = keyFrames[index].blendLoop;
        inatanceAnims[instanceID].blendFactor = 0.f;
        inatanceAnimCtrl[instanceID].fadeIn = fadeIn < keyFrames[index].duration ? fadeIn : keyFrames[index].duration - 1;
        inatanceAnimCtrl[instanceID].fadeOut = keyFrames[index].isLoop ? 0.f : (fadeOut < keyFrames[index].duration ? fadeOut : keyFrames[index].duration - 1);
    }
    void StopAni() { }
    void PauseAni() { }

    //void SetSpeed(UINT instanceID, float speed) { inatanceAnimCtrl[instanceID].speed = speed; }

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
	void StartInstancingMode();
	void IncreaseInstancing(Node* object);
	void DecreaseInstancing(Node* object);
	UINT instancingCount;
	bool bInstancingMode;

	VertexBuffer* instancingBuffer;
	Matrix worlds[MAX_MESH_INSTANCE];

	std::map<Node*, Matrix> instanceMatrixList;
private:
	Shader* csShader;
    void LoadModel(const string path);

    void CreateAnimSRV();

    void AnimUpdate();

    ////��ǻƮ ���̴��� ���̾��Ű ���̽� ��Ʈ
    vector<HierarchyNode> hierarchyNodes; //�ε����� ���ؼ��� ��������
    map<string,UINT> hierarchyMap;//���̾��Ű ���� �̿��ؼ� �̸����� ã�� ����
    ID3D11ShaderResourceView* hierarchyBufferSrv = nullptr;//���̾��Ű���� : preQuat, local(Ű���� ��Ʈ���� ��ȯ��), world(������ Ű�������� �����Ʈ����), offset, parentID, enable(��ǻƮ ���ʿ��� ������������)
    UINT nodeCount;

    //�޽� ������ ��Ʈ
    vector<Model::Mesh> meshs;
    vector<Material*> materials;
    map<string, UINT> materialMap;
    UINT stride = sizeof(VertexType);
    UINT offset = 0;

   
    //��ǻƮ ���̴��� �ν��Ͻ� �ִϸ��̼� ��Ʈ
    vector<ANIMATIONCTRL> inatanceAnimCtrl;//�ν��Ͻ��� �ִϸ��̼� ��Ʈ��(���ǵ�,���̵��ξƿ�,�̺�Ʈ ��)
    vector<INSTANCEANIMATION> inatanceAnims;//�ν��Ͻ��� �ִϸ��̼� ������
    ID3D11Buffer* inatanceAnimBuffer = nullptr;//��ǻƮ ���̴��� ����
    ID3D11ShaderResourceView* inatanceAnimSRV = nullptr;//���� �ؽ��ĸ� ��ǻƮ ���̴��� ������ �뵵

    //�ִϸ��̼� Ű������ ������ ��Ʈ
    vector<KEYFRAME> keyFrames;//�ҷ����� ��ü �ִϸ��̼� ����� ����

    ////��ǻƮ ���̴��� ���� ��Ʈ
    ID3D11ShaderResourceView* animSRV = nullptr;//���� Ű������ �����͸� ���� �ִϸ��̼� SRV
    ID3D11UnorderedAccessView* hierarchyMatrixUAV = nullptr;//��ǻƮ ���̴� �ӽ� ����� UAV
    vector<Matrix> hierarchyMatrix;//��ǻƮ ���̴� RW���� ���Ͻ� ����� ����� ��Ʈ���� ����
    ID3D11UnorderedAccessView* hierarchyUAV = nullptr;//��ǻƮ ���̴��� RW���� 2DArray�� ���� ���Ϳ��� ���� ���� �⺻���� �������·� �������
    ID3D11ShaderResourceView* hierarchySRV = nullptr;//���ؽ� ���̴��� ���̾��Ű SRV
};
