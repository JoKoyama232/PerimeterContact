//=============================================================================
//
// ���f������ [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "debugproc.h"
#include "model.h"
#include "light.h"
#include "skydome.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_SKY		"data/MODEL/skydome.obj"			// �ǂݍ��ރ��f����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static MDL		g_Sky;						// �v���C���[

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSky(void)
{
	//LoadModel(MODEL_SKY, &g_Sky.model, true);
	g_Sky.load = true;

	g_Sky.pos = { 0.0f, 0.0f, 0.0f };
	g_Sky.rot = { 0.0f, 0.0f, 0.0f };
	g_Sky.scl = { 11.0f, 11.0f, 11.0f };
	g_Sky.use = true;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSky(void)
{
	// ���f���̉������
	if (g_Sky.load)
	{
		UnloadModel(&g_Sky.model);
		g_Sky.load = false;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSky(void)
{

	g_Sky.rot.y += 0.0001f;
	{	// �|�C���g���C�g�̃e�X�g
		LIGHT* light = GetLightData(1);
		XMFLOAT3 pos = g_Sky.pos;
		pos.y -= 50.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = true;
		SetLightData(1, light);
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSky(void)
{
	SetLightEnable(false);
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Sky.scl.x, g_Sky.scl.y, g_Sky.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Sky.rot.x, g_Sky.rot.y + XM_PI, g_Sky.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Sky.pos.x, g_Sky.pos.y, g_Sky.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Sky.mtxWorld, mtxWorld);

	// ���f���`��
	DrawModel(&g_Sky.model);

	SetLightEnable(true);


	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}