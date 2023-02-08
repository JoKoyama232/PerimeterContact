//=============================================================================
//
// �e���ˏ��� [bullet.h]
// Author : ���R�@��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_BULLET		(256)	// �e�ő吔
#define	BULLET_WH		(5.0f)	// �����蔻��̑傫��

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	// 3D���f���ϐ��Q
	DX11_MODEL	model;		// ���f�����
	VERTEX_DATA	points;		// ���f���̒��_���
	bool		load;		// ���f���̓ǂݍ��݃t���O

	// �Q�[�����ʒu�ϐ��Q
	XMFLOAT4X4	mtxWorld;	// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;		// �|���S���̈ʒu
	XMFLOAT3	rot;		// �|���S���̌���(��])
	XMFLOAT3	scl;		// �|���S���̑傫��(�X�P�[��)

	// �Q�[�����W�b�N�ϐ��Q
	XMFLOAT3	startPos;	// �x�W�F�Ȑ��̎n�_
	XMFLOAT3	*target;	// �x�W�F�Ȑ��̏I�_�i�ǔ��ׁ̈j
	XMFLOAT3	targetPos;	// �x�W�F�Ȑ��̏I�_�i�ǔ����؂ꂽ���j		
	XMFLOAT3	middlePos;	// �x�W�F�Ȑ��̒��ԓ_
	
	DWORD		timer;		// �g�p�J�n������̎��ԁims�j
	float		speed;		// �o���b�g�̈�b�Ԃ̑��x
	float		progress;	// �o���b�g�̋O����̈ʒu
	bool		use;		// �g�p���Ă��邩�ǂ���

} BULLET;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 initialPos, XMFLOAT3 *targetPos, float speed);

BULLET *GetBullet(void);
XMFLOAT3 GetBezier(XMFLOAT3 initialPos, XMFLOAT3 middlePos, XMFLOAT3 endPos, float percentage);

