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
#include "enemy.h"
#include "shadow.h"
#include "meshfield.h"
#include "collision.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/player.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_PARTS	"data/MODEL/tracks.obj"

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ENEMY_OFFSET_Y		(40.0f)						// �G�l�~�[�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[
static ENEMY		g_Parts[ENEMY_PARTS_MAX];		// �G�l�~�[�̃p�[�c�p

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
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Enemy[i].hp = 20;
		g_Enemy[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

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
				g_Parts[j].pos = XMFLOAT3(30.0f, -ENEMY_OFFSET_Y * 2, -5.0f);
				g_Parts[j].rot = XMFLOAT3(XM_PI / 5, 0.0f, 0.0f);
				break;
			case 1:
				//Front Left
				g_Parts[j].pos = XMFLOAT3(-30.0f, -ENEMY_OFFSET_Y * 2, -5.0f);
				g_Parts[j].rot = XMFLOAT3(XM_PI / 5, 0.0f, 0.0f);
				break;
			case 2:
				//Back Right
				g_Parts[j].pos = XMFLOAT3(30.0f, -ENEMY_OFFSET_Y * 2, 0.0f);
				g_Parts[j].rot = XMFLOAT3(XM_PI / 5, XM_PI, 0.0f);
				break;
			case 3:
				//Back Left
				g_Parts[j].pos = XMFLOAT3(-30.0f, -ENEMY_OFFSET_Y * 2, 0.0f);
				g_Parts[j].rot = XMFLOAT3(XM_PI / 5, XM_PI, 0.0f);
				break;
			}
			g_Parts[j].scl = XMFLOAT3(2.0f, 2.0f, 2.0f);
			g_Parts[j].spd = 0.0f;
			g_Parts[j].size = ENEMY_SIZE;
			g_Parts[j].move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
			g_Parts[j].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			g_Parts[j].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			g_Parts[j].use = true;			// true:�����Ă�
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
			delete[] g_Enemy[i].points.VertexArray;
		}
		for (int j = 0; j < ENEMY_PARTS_MAX; j++) {
			if (g_Parts[j].load)
			{
				UnloadModel(&g_Parts[j].model);
				g_Parts[j].load = false;
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
		if (g_Enemy[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
		{								// ���`��Ԃ̏���
			// �ړ�����
			int		index = (int)g_Enemy[i].move_time;
			float	time = g_Enemy[i].move_time - index;
			int		size = g_Enemy[i].tbl_size;

			float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
			g_Enemy[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

			if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
			{
				g_Enemy[i].move_time = 0.0f;
				index = 0;
			}
			for (int j = 0; j < ENEMY_PARTS_MAX; j++) {
				if (!g_Parts[j].use) continue;

			}

			//// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
			//XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
			//XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
			//XMVECTOR vec = p1 - p0;
			//XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

			//// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
			//XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// ���̊p�x
			//XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
			//XMVECTOR rot = r1 - r0;
			//XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

			//// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
			//XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// ����Scale
			//XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// ���݂�Scale
			//XMVECTOR scl = s1 - s0;
			//XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

		}

		// �e���v���C���[�̈ʒu�ɍ��킹��
		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		
	}

	for (int i = 0; i < ENEMY_PARTS_MAX * MAX_ENEMY; i++)
	{
		XMFLOAT3 trackpos = AffineTransform(g_Parts[i].points.VertexArray[15], XMLoadFloat4x4(&g_Parts[i].mtxWorld));
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_WIREFRAME);

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
			mtxWorld = XMLoadFloat4x4(&g_Enemy[i].mtxWorld);

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