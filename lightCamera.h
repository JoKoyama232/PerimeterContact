// ���J�������� [lightCamera.h]
//����ҁF���R�@��
//��Ȗ����F�V���h�E�}�b�v
#pragma once


//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
struct CAMERA
{
	XMFLOAT4X4			mtxView;		// �r���[�}�g���b�N�X
	XMFLOAT4X4			mtxInvView;		// �r���[�}�g���b�N�X
	XMFLOAT4X4			mtxProjection;	// �v���W�F�N�V�����}�g���b�N�X

	XMFLOAT3			eye;			// �J�����̎��_(�ʒu)
	XMFLOAT3			at;				// �J�����̒����_
	XMFLOAT3			up;				// �J�����̏�����x�N�g��

	float				len;			// �J�����̎��_�ƒ����_�̋���

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitLightCamera(void);
void UninitLightCamera(void);
void UpdateLightCamera(void);
void SetLightCamera(void);

CAMERA* GetLightCamera(void);

void SetViewPort(int type);
int GetViewPortType(void);

void SetLightCameraAT(XMFLOAT3 pos);
