//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "shadow.h"
#include "meshfield.h"
#include "collision.h"
#include "debugproc.h"
#include "player.h"
#include "enemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/enemy.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_PARTS	"data/MODEL/tracks.obj"

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ENEMY_OFFSET_Y		(70.0f)						// �G�l�~�[�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

enum ENEMY_PATTERN {
	wait = 0,
	move,
	attack,
	run,
};
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[
static ENEMY		g_Parts[ENEMY_PARTS_MAX];		// �G�l�~�[�̃p�[�c�p
float				floating =0;
int g_Enemy_load = 0;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model,true,&g_Enemy[i].points);
		g_Enemy[i].load = true;

		g_Enemy[i].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Enemy[i].hitbox.positiona = { 0.0f, 3.0f, 0.0f };
		g_Enemy[i].hitbox.positionb = { 0.0f,2* ENEMY_OFFSET_Y-3.0f, 0.0f };
		g_Enemy[i].hitbox.radius = 3.0f;
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.5f, 1.5f, 1.5f);
		g_Enemy[i].hp = 50;
		g_Enemy[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��
		g_Enemy[i].gjkList.list =  new XMFLOAT3[g_Enemy[i].points.VertexNum];


		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		g_Enemy[i].move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Enemy[i].use = true;			// true:�����Ă�
		g_Enemy[i].attachedTo = NULL;

		//tracks
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{

			LoadModel(MODEL_ENEMY_PARTS, &g_Parts[j].model, true, &g_Parts[j].points);
			g_Parts[j].load = true;
			switch (j) {
			case 0:
				//Front Right
				g_Parts[j].pos = XMFLOAT3(20.0f, -20.0f, -5.0f);
				g_Parts[j].rot = XMFLOAT3(XM_PI / 5, 0.0f, 0.0f);
				break;
			case 1:
				//Front Left
				g_Parts[j].pos = XMFLOAT3(-20.0f, -20.0f, -5.0f);
				g_Parts[j].rot = XMFLOAT3(XM_PI / 5, 0.0f, 0.0f);
				break;
			case 2:
				//Back Right
				g_Parts[j].pos = XMFLOAT3(20.0f, -20.0f, 0.0f);
				g_Parts[j].rot = XMFLOAT3(XM_PI / 5, XM_PI, 0.0f);
				break;
			case 3:
				//Back Left
				g_Parts[j].pos = XMFLOAT3(-20.0f, -20.0f, 0.0f);
				g_Parts[j].rot = XMFLOAT3(XM_PI / 5, XM_PI, 0.0f);
				break;
			}
			g_Parts[j].scl = XMFLOAT3(1.5f, 1.5f, 1.5f);
			g_Parts[j].spd = 0.0f;
			g_Parts[j].size = ENEMY_SIZE;
			g_Parts[j].move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
			g_Parts[j].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			g_Parts[j].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			g_Parts[j].use = true;			// true:�����Ă�
			g_Parts[j].gjkList.list = new XMFLOAT3[g_Parts[j].points.VertexNum];
			g_Parts[j].attachedTo = &g_Enemy[i];


		}
	}
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = false;
			delete[] g_Enemy[i].gjkList.list;
			delete[] g_Enemy[i].points.VertexArray;
		}
		for (int j = 0; j < ENEMY_PARTS_MAX; j++) {
			if (g_Parts[j].load)
			{
				UnloadModel(&g_Parts[j].model);
				g_Parts[j].load = false;
				delete[] g_Parts[j].gjkList.list;
				delete[] g_Parts[j].points.VertexArray;
			}

		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use != true)	continue;		// ���̃G�l�~�[���g���Ă���H
													// Yes
		//if (GetKeyboardPress(DIK_P))
		//{	// �O�ֈړ�
		//	g_Enemy[i].pos.y -= 1.0f;
		//}

		g_Enemy[i].pos.x = sinf(floating) * 50.0f;
		g_Enemy[i].pos.z = cosf(floating) * 50.0f;
		floating += 0.01f;
		g_Enemy[i].rot.y += XM_PI * 0.01f;
		// �e���v���C���[�̈ʒu�ɍ��킹��
		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		PrintDebugProc("enemy:X:%f Y:%f Z:%f\n", g_Enemy[i].rot.x, g_Enemy[i].rot.y, g_Enemy[i].rot.z);
		UpdateHitbox(g_Enemy[i].gjkList.list, g_Enemy[i].points.VertexNum,
			g_Enemy[i].points.VertexArray, g_Enemy[i].pos, g_Enemy[i].rot, g_Enemy[i].scl, XMMatrixIdentity());

	}
	
	for (int i = 0; i < ENEMY_PARTS_MAX * MAX_ENEMY; i++)
	{
		if (!g_Parts[i].use)
			continue;
		UpdateHitbox(g_Parts[i].gjkList.list, g_Parts[i].points.VertexNum,
			g_Parts[i].points.VertexArray, g_Parts[i].pos, g_Parts[i].rot, g_Parts[i].scl, XMLoadFloat4x4(&g_Parts[i].attachedTo->mtxWorld));

		XMVECTOR rot = XMVector3Normalize(XMLoadFloat3(&g_Parts[i].rot));
		XMVECTOR pos = FindFurthestPointFrom(rot, g_Parts[i].gjkList.list, g_Parts[i].points.VertexNum);
		XMFLOAT3 posf3;
		XMStoreFloat3(&posf3, pos);
		XMFLOAT3 hitPos, hitNorm;
		RayHitField(posf3,&hitPos,&hitNorm);
			
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(GetRasterizerMode());

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Enemy[i].model);

		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			if (g_Parts[j].use == false) continue;

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Parts[j].scl.x, g_Parts[j].scl.y, g_Parts[j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[j].rot.x, g_Parts[j].rot.y + XM_PI, g_Parts[j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Parts[j].pos.x, g_Parts[j].pos.y, g_Parts[j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[j].attachedTo != NULL)
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].attachedTo->mtxWorld));
			}

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Parts[j].mtxWorld, mtxWorld);

			DrawModel(&g_Parts[j].model);
		}
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

ENEMY *GetEnemyParts(){
	return &g_Parts[0];
}