//=============================================================================
//
// �i�C�t���� [knife.h]
// Author : ���R�@��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_KNIFE		(10)					// �G�l�~�[�̐�

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
enum KNIFESTATE {
	unused = 0,
	fired,
	hit,
};

struct KNIFE
{
	// 3D���f���ϐ��Q
	DX11_MODEL	modelInfo;							// ���f�����
	XMFLOAT4	modelDiffuse[MODEL_MAX_MATERIAL];	// ���f���̐F
	VERTEX_DATA	modelVertexPosition;				// ���f���̒��_���W
	HITBOX		hitbox;								// �����蔻��p���W
	bool		load;								// ���f���̓ǂݍ��݃t���O

	// �Q�[�����ʒu�ϐ��Q
	XMFLOAT4X4	mtxWorld;							// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;								// ���f���̈ʒu
	XMFLOAT3	rot;								// ���f���̌���(��])
	XMFLOAT3	scl;								// ���f���̑傫��(�X�P�[��)

	// �Q�[�����W�b�N�ϐ��Q
	ENEMY* attachedTo;								// ���������G�l�~�[
	XMFLOAT3	velocity;							// �i�C�t�̑��x
	XMFLOAT3	acceleration;						// �i�C�t�̉����x
	KNIFESTATE	state;								// �i�C�t���݂̏��
	

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitKnife(void);
void UninitKnife(void);
void UpdateKnife(void);
void DrawKnife(void);

KNIFE* GetKnife(void);

