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
#define ENEMY_PARTS_MAX		(4)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	bool				use;
	bool				load;
	DX11_MODEL			model;				// ���f�����
	VERTEX_DATA			points;
	CAPSULEHITBOX		hitbox;
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F
	float				hp;
	float				spd;				// �ړ��X�s�[�h
	float				size;				// �����蔻��̑傫��
	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�
	
	INTERPOLATION_DATA	*tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tbl_size;			// �o�^�����e�[�u���̃��R�[�h����
	float				move_time;			// ���s����


	ENEMY* attachedTo;	// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

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
