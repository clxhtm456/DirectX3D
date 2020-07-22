#include "Math.hlsli"

struct HierarchyDATA
{
	float4 preQuat;
	matrix local;
	matrix world;
	matrix offset;
	int parentID;
	uint enable;
};
StructuredBuffer<HierarchyDATA> hierarchy : register(t0);

struct ANIMATION
{
	int clip;
	float curtime;
	float duration;
	float blendLoop;
};

struct INSTANCEANIMATION
{
	ANIMATION cur;
	ANIMATION next;

	float blendFactor;
};
StructuredBuffer<INSTANCEANIMATION> instanceAnim : register(t1);//�ν��Ͻִ̾ϸ��̼�

Texture2DArray animData : register(t2);//�ִϸ��̼� ������

RWTexture2D<float4> hierarchyMat : register(u0);
RWTexture2DArray<float4> output : register(u1);

void LoadkeyFrameData(uint id, ANIMATION clip, out float3 trans, out float4 quat, out float3 scale)
{
	float nextframe = fmod(clip.curtime + 1, clip.duration);
	float factor = frac(clip.curtime);
	trans = float3(animData.Load(int4(id * 3 + 0, floor(clip.curtime), clip.clip, 0)).xyz);
	quat = float4(animData.Load(int4(id * 3 + 1, floor(clip.curtime), clip.clip, 0)));
	scale = float3(animData.Load(int4(id * 3 + 2, floor(clip.curtime), clip.clip, 0)).xyz);
	trans = lerp(trans, animData.Load(int4(id * 3 + 0, floor(nextframe), clip.clip, 0)).xyz, factor);
	quat = slerp(quat, float4(animData.Load(int4(id * 3 + 1, floor(nextframe), clip.clip, 0))), factor);
	scale = lerp(scale, animData.Load(int4(id * 3 + 2, floor(nextframe), clip.clip, 0)).xyz, factor);

	[flatten]
	if (clip.duration - clip.curtime < clip.blendLoop)
	{
		factor = (clip.duration - clip.curtime) / clip.blendLoop;
		trans = lerp(animData.Load(int4(id * 3 + 0, 0, clip.clip, 0)).xyz, trans, factor);
		quat = slerp(float4(animData.Load(int4(id * 3 + 1, 0, clip.clip, 0))), quat, factor);
		scale = lerp(animData.Load(int4(id * 3 + 2, 0, clip.clip, 0)).xyz, scale, factor);
	}
}

void SaveHierarchyMatrix(uint id, uint gid, float4x4 inputMatrix, uint keyType)
{
	hierarchyMat[uint2(id * 4 + 0, gid)] = float4(inputMatrix._11_12_13, float(keyType));
	hierarchyMat[uint2(id * 4 + 1, gid)] = inputMatrix._21_22_23_24;
	hierarchyMat[uint2(id * 4 + 2, gid)] = inputMatrix._31_32_33_34;
	hierarchyMat[uint2(id * 4 + 3, gid)] = inputMatrix._41_42_43_44;
}

uint LoadkeyType(uint id, uint gid)
{
	return uint(hierarchyMat.Load(int3(id * 4 + 0, gid, 0)).w);
}

float4x4 LoadHierarchyMatrix(uint id, uint gid)
{
	return float4x4(float4(hierarchyMat.Load(int3(id * 4 + 0, gid, 0)).xyz, 0),
					hierarchyMat.Load(int3(id * 4 + 1, gid, 0)),
					hierarchyMat.Load(int3(id * 4 + 2, gid, 0)),
					hierarchyMat.Load(int3(id * 4 + 3, gid, 0)));
}

void WriteRWMatrix(uint id, uint gid, float4x4 inputMatrix)
{
	output[uint3(id * 4 + 0, 0, gid)] = inputMatrix._11_12_13_14;
	output[uint3(id * 4 + 1, 0, gid)] = inputMatrix._21_22_23_24;
	output[uint3(id * 4 + 2, 0, gid)] = inputMatrix._31_32_33_34;
	output[uint3(id * 4 + 3, 0, gid)] = inputMatrix._41_42_43_44;
}

[numthreads(32, 32, 1)]
void CS(uint ID : SV_GroupIndex, uint3 GID : SV_GroupID)//GID �ν��Ͻ� �ε���
{
	matrix curmatrix = 0;
	uint keyType = 0;

	if (hierarchy[ID].enable > 0)
	{
		keyType = uint(animData.Load(int4(ID * 3, 0, instanceAnim[GID.x].cur.clip, 0)).w);
		//ŰŸ���� nextŬ���� ���� ū�ʺ�ȯ ŰŸ�� 0�� ������ RW��Ʈ���� �Է� �״�� ��. ŰŸ�� 1�� ���ÿ����Է�. ŰŸ��2�� �ִϸ��̼� ��Ʈ���� ��ȯ
		[flatten]
		if (instanceAnim[GID.x].cur.clip != instanceAnim[GID.x].next.clip && instanceAnim[GID.x].blendFactor > 0.0F)
			keyType = max(keyType, uint(animData.Load(int4(ID * 3, 0, instanceAnim[GID.x].next.clip, 0)).w));

		[flatten]
		if (keyType > 1)
		{
			float3 trans;
			float4 quat;
			float3 scale;
			LoadkeyFrameData(ID, instanceAnim[GID.x].cur, trans, quat, scale);
			
			//�ؽ�Ʈ Ÿ�Ӱ��� ����
			[flatten]
			if (instanceAnim[GID.x].cur.clip != instanceAnim[GID.x].next.clip && instanceAnim[GID.x].blendFactor > 0.0F)
			{
				float3 nexttrans;
				float4 nextquat;
				float3 nextscale;
				LoadkeyFrameData(ID, instanceAnim[GID.x].next, nexttrans, nextquat, nextscale);
				trans = lerp(trans, nexttrans, instanceAnim[GID.x].blendFactor);
				quat = slerp(quat, nextquat, instanceAnim[GID.x].blendFactor);
				scale = lerp(scale, nextscale, instanceAnim[GID.x].blendFactor);
			}

			quat = mulQuat(quat, hierarchy[ID].preQuat);

			curmatrix = calcMatrix(trans, quat, scale);
	
		}
		else
		{
			[flatten]
			if (keyType == 1)//ŰŸ���� 1�̸� ������ �Է¹���
				curmatrix = hierarchy[ID].local;
		}

		SaveHierarchyMatrix(ID, GID.x, curmatrix, keyType);
	}
	
	GroupMemoryBarrier();
	
	[flatten]
	if (hierarchy[ID].enable == 0 || keyType == 0)// || keyType == 0
		return;
	
	int parentID = hierarchy[ID].parentID;
	uint parentKeyType = LoadkeyType(parentID, GID.x);

	while (parentID > -1)
	{
		[flatten]
		if (parentKeyType == 0)//��ƮŰŸ���� 0�̵Ǹ� ���带 ���ؼ� �������� ��������
		{
			curmatrix = mul(curmatrix, hierarchy[parentID].world);
			break;
		}
		curmatrix = mul(curmatrix, LoadHierarchyMatrix(parentID, GID.x));
		parentID = hierarchy[parentID].parentID;
		parentKeyType = LoadkeyType(parentID, GID.x);
	}

	//offset��Ʈ���� ���. ���� �ƴҰ�� offset��Ʈ������ ���� ����̱⿡ ��ȭ ����
	curmatrix = mul(hierarchy[ID].offset, curmatrix);
	
	WriteRWMatrix(ID, GID.x, curmatrix);//2DArray�� Array�� �ν��Ͻ� �ε����� ����. 2D�� X�� ���̾��Ű�� ���� Y�� ��Ǻ��� ���� ���ܵ�. ��Ǻ��� ��Ʈ���� ������� Y=0�� ���� ���������� ������ �����
	//ex)		1frame				2frame				3frame				4frame				5frame				6frame.......
	//y=0		1frameMatrix		2frameMatrix		3frameMatrix		4frameMatrix		5frameMatrix		6frameMatrix.....
	//y=1		identitymatrix		1frameMatrix		2frameMatrix		3frameMatrix		4frameMatrix		5frameMatrix.....
	//y=2		identitymatrix		identitymatrix		1frameMatrix		2frameMatrix		3frameMatrix		4frameMatrix.....
	//y=3		identitymatrix		identitymatrix		identitymatrix		1frameMatrix		2frameMatrix		3frameMatrix.....
	//y=4		identitymatrix		identitymatrix		identitymatrix		identitymatrix		1frameMatrix		2frameMatrix.....
	//y=5		identitymatrix		identitymatrix		identitymatrix		identitymatrix		identitymatrix		1frameMatrix.....
	//y=6		identitymatrix		identitymatrix		identitymatrix		identitymatrix		identitymatrix		identitymatrix.....
	//y=7		identitymatrix		identitymatrix		identitymatrix		identitymatrix		identitymatrix		identitymatrix.....
	//y=8		identitymatrix		identitymatrix		identitymatrix		identitymatrix		identitymatrix		identitymatrix.....
	//y=9		identitymatrix		identitymatrix		identitymatrix		identitymatrix		identitymatrix		identitymatrix.....
	//10�����ӱ��� ���. �ϴ� ������ �������. ������ �� �κ� ��Ʈ���� ����� ���ص� ������ ���ؽ� ���̴����� ����� ���� ������.
	//������ ����å - ���ؽ� ���̴� ��ü���� ���� ���ؽ� �������� RW�� ���� ������ ������ ��� ���� ���� ���ο� �����ӿ� ������ ��� ���徿 �о� �־� ���
	//�ȼ����̴����� ���徿 �о���� ��Ͽ� ���� �ش� ���ؽ� ��ġ���� �̵� �Ÿ��� ������ ������ ���͸ʻ���
	//����Ʈ �������� �̹��� ��ü�� ���� ���͸��� �����Ͽ� �̵��� �����Ӹ�ŭ ��Ǻ� ����
	//����å���� �Ұ�� ��ǻƮ ���̴� output���� 2Darray�� �ʿ䰡 ������. 1DArray�� 2D�� ��� ����(x = ���̾��Ű��� ����, y = instance). ��� ���ؽ� ���̴����� 2DArray����ؾ���(x = ���ؽ�,y = ������, z = instance)
	//���� ���������� �������Ӱ��� �̵��� ����Ұ��. 1DArray�� 2D�� ��� ����(x = ���ؽ�, y = instance). ���ڿ������� ��Ǻ�
}