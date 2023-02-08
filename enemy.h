//=============================================================================
//
// �G�l�~�[���f������ [enemy.h]
// Author : ���R�@��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_ENEMY		(1)					// �G�l�~�[�̐�
#define	ENEMY_SIZE		(5.0f)				// �����蔻��̑傫��
#define ENEMY_PARTS_MAX	(4)					// �G�l�~�[�̕��i��

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY
{
	// 3D���f���ϐ��Q
	DX11_MODEL			model;							// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F
	VERTEX_DATA			points;							// ���f���̒��_���W
	HITBOX				gjkList;						// �����蔻��p���W
	CAPSULEHITBOX		hitbox;							// �J�v�Z���R���W�����p�ϐ�
	bool				load;							// ���f���̓ǂݍ��݃t���O

	// �Q�[�����ʒu�ϐ��Q
	XMFLOAT4X4			mtxWorld;						// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;							// ���f���̈ʒu
	XMFLOAT3			rot;							// ���f���̌���(��])
	XMFLOAT3			scl;							// ���f���̑傫��(�X�P�[��)
	int					tbl_size;						// �o�^�����e�[�u���̃��R�[�h����
	INTERPOLATION_DATA* tbl_adr;						// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	ENEMY* attachedTo;	// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �Q�[�����W�b�N�ϐ��Q
	float				hp;								// �q�b�g�|�C���g
	float				spd;							// �ړ��X�s�[�h
	float				size;							// �����蔻��̑傫��
	int					shadowIdx;						// �e�̃C���f�b�N�X�ԍ�
	float				move_time;						// ���s����
	bool				use;							// �`��t���O
};

struct ENEMYHitBox {
	HITBOX* hitbox;
	ENEMY* attachedTo;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
ENEMY* GetEnemyParts();
