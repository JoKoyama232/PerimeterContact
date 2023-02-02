//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "3DParticle.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_PARTICLE3D	"data/MODEL/smoke.obj"		// �ǂݍ��ރ��f����

#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PARTICLE3D		g_Particle3D[MAX_PARTICLE3D];				// �G�l�~�[

//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticle3D(void)
{
	for (int i = 0; i < MAX_PARTICLE3D; i++)
	{
		LoadModel(MODEL_PARTICLE3D, &g_Particle3D[i].modelInfo, false, new VERTEX_DATA);
		GetModelDiffuse(&g_Particle3D[i].modelInfo, &g_Particle3D[i].modelDiffuse[0]);
		g_Particle3D[i].load = true;

		g_Particle3D[i].pos = XMFLOAT3(0.0f, -20.0f, 20.0f);
		g_Particle3D[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Particle3D[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Particle3D[i].showTime = 0.0f;
		g_Particle3D[i].Center = false;
		g_Particle3D[i].use = false;

	}
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitParticle3D(void)
{

	for (int i = 0; i < MAX_PARTICLE3D; i++)
	{
		if (g_Particle3D[i].load)
		{
			UnloadModel(&g_Particle3D[i].modelInfo);
			g_Particle3D[i].load = false;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateParticle3D(void)
{
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_PARTICLE3D; i++)
	{
		if (g_Particle3D[i].use != true)	
			continue;		// ���̃G�l�~�[���g���Ă���H
	
		g_Particle3D[i].showTime--;
		g_Particle3D[i].rot.y += 0.01f;
		g_Particle3D[i].rot.z += 0.01f;
		g_Particle3D[i].rot.x += 0.01f;
		if (g_Particle3D[i].Center) 
		{
			Particle3DCenterExplsosion(&g_Particle3D[i]);
		}


	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawParticle3D(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_BACK);

	for (int i = 0; i < MAX_PARTICLE3D; i++)
	{
		if (g_Particle3D[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Particle3D[i].scl.x, g_Particle3D[i].scl.y, g_Particle3D[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Particle3D[i].rot.x, g_Particle3D[i].rot.y, g_Particle3D[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Particle3D[i].pos.x, g_Particle3D[i].pos.y, g_Particle3D[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Particle3D[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Particle3D[i].modelInfo);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �p�[�e�B�N���̐ݒ�
//=============================================================================
void SetParticle3D(XMFLOAT3 position, XMFLOAT3 deltaPosition, float ShowTime, bool center) 
{
	for (int i = 0; i < MAX_PARTICLE3D; i++)
	{
		if (g_Particle3D[i].use)
			continue;

		g_Particle3D[i].Center = center;
		g_Particle3D[i].pos = position;
		g_Particle3D[i].velocity = deltaPosition;
		g_Particle3D[i].showTime = 16;
		g_Particle3D[i].use = true;
	}
}

void Particle3DCenterExplsosion(PARTICLE3D *particle) {
	XMVECTOR size = {1.0f,1.0f,1.0f};
	if (particle->showTime < 0) 
	{ 
		particle->use = false;
		return; 
	}
	
	if (particle->showTime > 8) {
		size *= (8 - particle->showTime - 8) / 8 * 3.0f;
		XMStoreFloat3(&particle->scl, size);
		return;
	}
	if (particle->showTime < 8) {

	}
	
	
}