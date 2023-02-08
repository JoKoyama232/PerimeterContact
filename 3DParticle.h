//=============================================================================
//
// �G�l�~�[���f������ [Particle3D.h]
// Author : ���R�@��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PARTICLE3D		(255)	// �G�l�~�[�̐�

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PARTICLE3D
{
	// 3D���f���ϐ��Q
	DX11_MODEL	modelInfo;			// ���f�����
	XMFLOAT4	modelDiffuse[MODEL_MAX_MATERIAL];	// ���f���̐F
	bool		load;				// ���f�����ǂݍ��݃t���O

	// �Q�[�����ʒu�ϐ��Q
	XMFLOAT4X4	mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;				// ���f���̈ʒu
	XMFLOAT3	rot;				// ���f���̌���(��])
	XMFLOAT3	scl;				// ���f���̑傫��(�X�P�[��)
	XMFLOAT3	velocity;			// ���f���̑��x	
	
	// �Q�[�����W�b�N�ϐ��Q
	float		showTime;			// �`�掞�ԁi�t���[�����j
	bool		Center;				// ���S�����t���O
	bool		use;				// �`��t���O
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticle3D(void);
void UninitParticle3D(void);
void UpdateParticle3D(void);
void DrawParticle3D(void);

void SetParticle3D(XMFLOAT3 position, XMFLOAT3 deltaPosition, float ShowTime, bool center);
void Particle3DCenterExplsosion(PARTICLE3D* particle);
