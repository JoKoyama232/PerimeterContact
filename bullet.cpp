//=============================================================================
//
// �e���ˏ��� [bullet.cpp]
// Author : ���R�@��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "shadow.h"
#include "collision.h"
#include "model.h"
#include "camera.h"
#include "bullet.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define MODEL_BULLET		"data/MODEL/missle.obj"
#define	BULLET_SPEED		(5.0f)			// �e�̈ړ��X�s�[�h


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexBullet(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

static BULLET						g_Bullet[MAX_BULLET];	// �e��

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
{


	// �e���[�N�̏�����
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		LoadModel(MODEL_BULLET, &g_Bullet[nCntBullet].model, true, &g_Bullet[nCntBullet].points);
		g_Bullet[nCntBullet].pos = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].rot = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].targetPos = { 1.0f, 1.0f, 1.0f };
		g_Bullet[nCntBullet].scl = { 1.0f,1.0f,1.0 };
		g_Bullet[nCntBullet].use = false;

	}


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBullet(void)
{
	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].load)
		{
			UnloadModel(&g_Bullet[i].model);
			g_Bullet[i].load = false;
		}

	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBullet(void)
{

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
			XMVECTOR prevPos = XMLoadFloat3(&g_Bullet[i].pos);
			XMVECTOR curPos;
			XMVECTOR directionVector;
			DWORD deltaTimeMs = timeGetTime() - g_Bullet[i].timer;
			g_Bullet[i].timer = timeGetTime();
			if (g_Bullet[i].progress < 0.9f) {
				g_Bullet[i].targetPos = { g_Bullet[i].target->x, g_Bullet[i].target->y, g_Bullet[i].target->z };
			}

			g_Bullet[i].pos = GetBezier(g_Bullet[i].startPos, g_Bullet[i].middlePos, g_Bullet[i].targetPos, g_Bullet[i].progress);
			curPos = XMLoadFloat3(&g_Bullet[i].pos);
			directionVector =XMVector3Normalize( curPos - prevPos);

			if(g_Bullet[i].progress > 0.5f)g_Bullet[i].pos.y +=  25.0f*sinf(g_Bullet[i].progress * 8 *XM_PI);
			g_Bullet[i].progress +=  0.001f;

			if (g_Bullet[i].progress > 1.0f) 
			{ 
				g_Bullet[i].use = false; 
			}
			// �e�̈ʒu�ݒ�


			// �t�B�[���h�̊O�ɏo����e����������
			if (g_Bullet[i].pos.x < MAP_LEFT
				|| g_Bullet[i].pos.x > MAP_RIGHT
				|| g_Bullet[i].pos.z < MAP_DOWN
				|| g_Bullet[i].pos.z > MAP_TOP)
			{
				g_Bullet[i].use = false;
			}

		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBullet(void)
{
	SetLightEnable(false);

	// �J�����O����
	SetCullingMode(CULL_MODE_BACK);

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{

			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Bullet[i].scl.x, g_Bullet[i].scl.y, g_Bullet[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Bullet[i].rot.x, g_Bullet[i].rot.y + XM_PI, g_Bullet[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Bullet[i].pos.x, g_Bullet[i].pos.y, g_Bullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Bullet[i].mtxWorld, mtxWorld);


			// ���f���`��
			DrawModel(&g_Bullet[i].model);
			XMFLOAT3* playerVerts = new XMFLOAT3[g_Bullet[i].points.VertexNum];
			for (int p = 0; p < g_Bullet[i].points.VertexNum; p++) {
				playerVerts[p] = AffineTransform(g_Bullet[i].points.VertexArray[p], XMLoadFloat4x4(&g_Bullet[i].mtxWorld));
			}

		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(true);

}


//=============================================================================
// �e�̃p�����[�^���Z�b�g
//=============================================================================
int SetBullet(XMFLOAT3 initialPos, XMFLOAT3 *targetPos,float speed)
{
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		if (!g_Bullet[nCntBullet].use)
		{
			g_Bullet[nCntBullet].startPos = initialPos;
			g_Bullet[nCntBullet].target = targetPos;
			g_Bullet[nCntBullet].timer = timeGetTime();
			XMVECTOR base = XMLoadFloat3(&initialPos);
			XMFLOAT3 direction = { (g_Bullet[nCntBullet].target->x - initialPos.x),0.0f,(g_Bullet[nCntBullet].target->y- initialPos.x) };
			XMVECTOR normalDirectionVector = XMVector3Normalize( XMLoadFloat3(&direction));
			XMVECTOR up = { (0.0f,1.0f,0.0f) };
			XMVECTOR side;
			crossProduct(&side, &normalDirectionVector, &up);
			XMVECTOR middlePoint = base - 200 * normalDirectionVector + (rand() % 10 - 5) * side;
			XMStoreFloat3(&g_Bullet[nCntBullet].middlePos, middlePoint);
			g_Bullet[nCntBullet].middlePos.y = g_Bullet[nCntBullet].target->y + 100.0f;
			g_Bullet[nCntBullet].progress = 0.0f;
			g_Bullet[nCntBullet].scl = { 1.0f,1.0f,1.0 };
			g_Bullet[nCntBullet].use = true;

			nIdxBullet = nCntBullet;

		//	PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxBullet;
}

//=============================================================================
// �e�̎擾
//=============================================================================
BULLET *GetBullet(void)
{
	return &(g_Bullet[0]);
}

XMFLOAT3 GetBezier(XMFLOAT3 initialPos, XMFLOAT3 middlePos, XMFLOAT3 endPos, float percentage) {
	XMFLOAT3 result = initialPos;
	float invP = 1 - percentage;
	result.x = invP * invP * initialPos.x + invP * 2 * percentage * middlePos.x + percentage * percentage * endPos.x;
	result.y = invP * invP * initialPos.y + invP * 2 * percentage * middlePos.y + percentage * percentage * endPos.y;
	result.z = invP * invP * initialPos.z + invP * 2 * percentage * middlePos.z + percentage * percentage * endPos.z;
	return result;

}

