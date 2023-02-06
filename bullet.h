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
	XMFLOAT3			pos;		// �|���S���̈ʒu
	XMFLOAT3			rot;		// �|���S���̌���(��])
	XMFLOAT3			scl;		// �|���S���̑傫��(�X�P�[��)

	XMFLOAT4X4			mtxWorld;	// ���[���h�}�g���b�N�X

	bool				load;
	DX11_MODEL			model;		// ���f�����
	VERTEX_DATA			points;


	XMFLOAT3	*target;
	XMFLOAT3	targetPos;			// �X�P�[��
	XMFLOAT3	startPos;
	XMFLOAT3	middlePos;
	

	DWORD		timer;
	float		speed;
	float		progress;
	bool		use;			// �g�p���Ă��邩�ǂ���

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

