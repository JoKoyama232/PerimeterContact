//=============================================================================
//
// �i�C�t���� [knife.cpp]
// Author : ���R�@��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "model.h"
#include "collision.h"
#include "enemy.h"
#include "player.h"
#include "knife.h"
#include "3DParticle.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_KNIFE			"data/MODEL/sword.obj"		// ���f����
#define	MODEL_KNIFE_PARTS	"data/MODEL/tracks.obj"		

#define	ROTATE_VALUE		(XM_PI * 0.1f)				// ��]��
#define	THROW_DIRECTION_POW	(250)				// ��]��
#define	THROW_HEIGHT_POW	(100.0f)				// ��]��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static KNIFE			g_Knife[MAX_KNIFE];				// �i�C�t
static int				hitKnife = 0;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitKnife(void)
{
	for (int i = 0; i < MAX_KNIFE; i++)
	{
		// 3D���f��
		LoadModel(MODEL_KNIFE, &g_Knife[i].modelInfo,true,&g_Knife[i].modelVertexPosition);
		GetModelDiffuse(&g_Knife[i].modelInfo, &g_Knife[0].modelDiffuse[i]);
		g_Knife[i].load = true;

		// �Q�[�����ʒu
		g_Knife[i].pos = XMFLOAT3(0.0f, -20.0f, 0.0f);
		g_Knife[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Knife[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);
		
		// �Q�[���p���W�b�N
		g_Knife[i].attachedTo = NULL;
		g_Knife[i].velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Knife[i].acceleration = XMFLOAT3(0.0f, -10.0f, 0.0f);
		g_Knife[i].state = unused;
		
	}
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitKnife(void)
{
	for (int i = 0; i < MAX_KNIFE; i++)
	{
		if (!g_Knife[i].load) continue;
		
		UnloadModel(&g_Knife[i].modelInfo);
		g_Knife[i].load = false;
		delete[] g_Knife[i].modelVertexPosition.VertexArray;
		delete[] g_Knife[i].hitbox.list;
		
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateKnife(void)
{
	hitKnife = 0;
	// �i�C�t�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_KNIFE; i++)
	{
		if (g_Knife[i].state == hit){
			UpdateHitbox(g_Knife[i].hitbox.list, g_Knife[i].modelVertexPosition.VertexNum,
				g_Knife[i].modelVertexPosition.VertexArray, g_Knife[i].pos, g_Knife[i].rot, g_Knife[i].scl,XMMatrixIdentity());
			continue;
		}
			
		
		// ��΂���Ă���Ȃ畨���v�Z
		if (g_Knife[i].state != fired)	continue;
		if (g_Knife[i].pos.y < -50.0f) g_Knife[i].state = unused;

		DWORD currentTimems = timeGetTime();
		DWORD elapsedTimems = currentTimems - g_Knife[i].latestUpdate;
		g_Knife[i].latestUpdate = currentTimems;

		// �ړ�����
		XMVECTOR position = XMLoadFloat3(&g_Knife[i].pos);
		XMVECTOR velocity = XMLoadFloat3(&g_Knife[i].velocity);
		XMVECTOR acceleration = XMLoadFloat3(&g_Knife[i].acceleration);

		position += velocity * (float)(elapsedTimems * 0.001f);
		velocity += acceleration * (float)(elapsedTimems * 0.01f);

		XMStoreFloat3(&g_Knife[i].pos,position);
		XMStoreFloat3(&g_Knife[i].velocity, velocity);

		// ��]����
		g_Knife[i].rot.x -= ROTATE_VALUE;

		// �q�b�g�{�b�N�X
		UpdateHitbox(g_Knife[i].hitbox.list, g_Knife[i].modelVertexPosition.VertexNum,
			g_Knife[i].modelVertexPosition.VertexArray, g_Knife[i].pos, g_Knife[i].rot, g_Knife[i].scl,XMMatrixIdentity());
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawKnife(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(GetRasterizerMode());

	for (int i = 0; i < MAX_KNIFE; i++)
	{
		if (g_Knife[i].state == unused) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();
		
		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Knife[i].scl.x, g_Knife[i].scl.y, g_Knife[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Knife[i].rot.x, g_Knife[i].rot.y, g_Knife[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Knife[i].pos.x, g_Knife[i].pos.y, g_Knife[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Knife[i].state == hit && g_Knife[i].attachedTo != NULL) {
			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(-g_Knife[i].parentRot.z, -g_Knife[i].parentRot.y +XM_PI, -g_Knife[i].parentRot.x);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Knife[i].attachedTo->mtxWorld));

		}
		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Knife[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Knife[i].modelInfo);

	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �i�C�t�̎擾
//=============================================================================
KNIFE *GetKnife()
{
	return &g_Knife[0];
}

//=============================================================================
// �i�C�t���΂�
//=============================================================================
void FireKnife(XMFLOAT3 firePosition,XMFLOAT3 fireDirection) {
	for (int i = 0; i < MAX_KNIFE; i++)
	{
		if (!g_Knife[i].load || g_Knife[i].state != unused) continue;

		g_Knife[i].hitbox.list = new XMFLOAT3[g_Knife[i].modelVertexPosition.VertexNum];
		g_Knife[i].latestUpdate = timeGetTime();
		g_Knife[i].pos = firePosition;
		g_Knife[i].rot.y = fireDirection.y;
		g_Knife[i].velocity.x = sinf(fireDirection.y)* THROW_DIRECTION_POW;
		g_Knife[i].velocity.z = cosf(fireDirection.y)* THROW_DIRECTION_POW;
		g_Knife[i].velocity.y = sinf(fireDirection.x)* THROW_HEIGHT_POW+ THROW_HEIGHT_POW;
		g_Knife[i].state = fired;
		return;

	}
}

void explodeKnife() {
	for (int i = 0; i < MAX_KNIFE; i++)
	{
		if (g_Knife[i].state != hit) continue;
		XMMATRIX mtxRot, mtxWorld;
		mtxWorld = XMMatrixIdentity();
		mtxRot = XMMatrixRotationRollPitchYaw(-g_Knife[i].parentRot.z, -g_Knife[i].parentRot.y + XM_PI, -g_Knife[i].parentRot.x);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
		mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Knife[i].attachedTo->mtxWorld));
		SetParticle3D(AffineTransform(g_Knife[i].pos, mtxWorld), { 0.0f,1.0f,0.0f }, 20, true);
		g_Knife[i].state = unused;
		g_Knife[i].scl = { 0.5f,0.5f,0.5f };

	}
}