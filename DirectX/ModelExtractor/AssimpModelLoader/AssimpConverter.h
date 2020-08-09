#pragma once

//Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#pragma comment(lib, "assimp/assimp-vc142-mtd.lib")

class AssimpConverter
{
private:
    struct HierarchyNode
    {
        string name;

        XMFLOAT4 translate = { 0.f,0.f,0.f,0.0f };
        XMFLOAT4 preQuaternion = { 0.f,0.f,0.f,1.f };
        XMFLOAT4 quaternion = { 0.f,0.f,0.f,1.f };
        XMFLOAT4 scale = { 1.f,1.f,1.f,1.0f };
        XMMATRIX local = XMMatrixIdentity();
        XMMATRIX world = XMMatrixIdentity();
		XMMATRIX offset;
        int parentID = -1;
        HierarchyNode()
        {
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
        UINT ID = 0;
        UINT matrialID = 0;
        vector<VertexType> vertices;
        vector<UINT> indices;
    };

    struct Material
    {
        string name;
        UINT ID = 0;

        XMFLOAT3 diffuse = { 0.f,0.f,0.f };
        XMFLOAT3 ambient = { 0.f,0.f,0.f };
        XMFLOAT3 specular = { 0.f,0.f,0.f };
        XMFLOAT3 emissive = { 0.f,0.f,0.f };
        XMFLOAT3 tranparent = { 1.f,1.f,1.f };
        XMFLOAT3 reflective = { 0.f,0.f,0.f };

        FLOAT opacity = 0.f;
        FLOAT transparentfactor = 0.f;
        FLOAT bumpscaling = 0.f;
        FLOAT shininess = 0.f;
        FLOAT reflectivity = 0.f;
        FLOAT shininessstrength = 0.f;
        FLOAT refracti = 0.f;

        string diffusefile;
        string specularfile;
        string ambientfile;
        string emissivefile;
        string heightfile;
        string normalfile;
        string shininessfile;
        string opacityfile;
        string displacementfile;
        string lightMapfile;
        string reflectionfile;
        string basecolorfile;
        string normalcamerafile;
        string emissioncolorfile;
        string metalnessfile;
        string diffuseroughnessfile;
        string ambientocculsionfile;
    };

    struct Key
    {
		float time = 0.0f;

        XMFLOAT4 translate = { 0.f,0.f,0.f ,0.0f};
        XMFLOAT4 quaternion = { 0.f,0.f,0.f,1.f };
		XMFLOAT4 scale = { 1.f,1.f,1.f ,0.0f};
    };

	struct KeyFrame
	{
		string boneName;

		vector<Key> keys;
	};

    struct Animation
    {
        string name;

        float tickPerSec = 0.f;
        float duration = 0.f;

        vector<KeyFrame> keyframes;//��帶�� ��ü Ű�����Ӻ� Ű���� ��������.������ ��ǻƮ���̴����� ��Ʈ������ Ű�����Ӹ��� ��ȯ ��ų���� �������� ��Ȯ�� �������� �Ҽ������� ������Ű�°� ����.
        //�ٱ����� ������ ī��Ʈ�� ��ü Ű������, ������ ��ü ���̾��Ű��� ���� ����Ʈ ���� ����� ��Ʈ���� �ѱ�� �ͺ��ٴ� �ð��� �ɸ��� ���� slerp�� �� ���� ����!
    };

	
public:
    AssimpConverter();
    ~AssimpConverter();

    void ConvertMesh(const string path, const string outPath = "");
    void LoadAnimation(const string path);
    void SaveAnimation(string path = "", UINT takeNum = 0);
private:
    void LoadMesh();
    void SaveMesh();//���̾��Ű���� �޽�����, ���͸�������, ������ ����

    void ResetMeshs();

    void NodeHeirarchy(const aiNode* node, int value);
    void InitBones(const aiScene* scene);
    void InitMesh(const aiScene* scene);
    void BindSkin(const aiMesh* mesh, vector<VertexType>& vertices);

    UINT InitMaterials(const aiScene* scene, UINT index);
    string SaveTexture(const aiScene* scene, string file);

    void ResetAnimations();

    void AddAnimation(const aiScene* scene);
    void CalcInterpolatedPosition(const aiNodeAnim* nodeAnim, const UINT frame, XMFLOAT4& trans);
    void CalcInterpolatedQuaternion(const aiNodeAnim* nodeAnim, const UINT frame, XMFLOAT4& quat);
    void CalcInterpolatedScaling(const aiNodeAnim* nodeAnim, const UINT frame, XMFLOAT4& scale);


    //map���� vector�ӵ��� ������ ���� vector�� ��� vector�� ��Ī�� find_if(nodeList.begin(), nodeList.end(), [bName](const NodeInfo* a)->bool { return a->name == bName; });
    //map�� �ӵ����غ��� ��. �뷫 ��� 200������ 300�� ����Ʈ �����ϰ� �ӵ� ���غ����� �ִ�ġ 600������(512��)
    //(ex: �巡��׽�Ʈ �巡�� 338��, �¿����� ���ۻ����� 250��, ���̺� ���϶� 423��, �� �������� 519��)
    //Vector���� map�� �� ���� ��;;;
    vector<HierarchyNode> hierarchyNodes;//��� �ִϸ��̼� Ű������ ���̾��Ű�󿡼� �̷����
    map<string, UINT> hierarchyMap;
    //vector<UINT> bones;//���� ���� ��� �޽��� ������ �ִ� ����Ʈ�� ����. ���ؽ����̴����� ���� ��Ʈ���� Ȥ�� SRT�� �ּ� ����
    map<UINT, UINT> meshMap;
    vector<Mesh> meshs;//�������� ���Ǵ� ���� �޽�������. ������ ���͸����� ������ �޽��� �и��Ǿ� ���͸��󸶴� �ϳ����� �޽��� ����. ���� ���͸����� ���� �޽��� �ִ� ��쵵 ����
    vector<Material> materials;//������ �޽����� ����ϴ� ���͸��� ����, �޽��� �ִ� ���͸��� ����
    string name = "";
    string animName = "";
    string meshPath = "";
    string outPath = "";
    vector<Animation> animations;
};

