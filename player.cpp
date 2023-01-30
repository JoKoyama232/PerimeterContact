//=============================================================================
//
// ���f������ [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "particle.h"
#include "meshfield.h"
#include "collision.h"
#include "enemy.h"
#include "knife.h"
#include "player.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/enemy.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_HAND	"data/MODEL/hand.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_HEAD	"data/MODEL/head.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS	"data/MODEL/funnel_s.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_1	"data/MODEL/funnel_f.obj"			// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(4.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define PLAYER_SHADOW_SIZE	(1.0f)							// �e�̑傫��
#define PLAYER_OFFSET_Y		(10.0f)							// �v���C���[�̑��������킹��

#define PLAYER_PARTS_MAX	(7)								// �v���C���[�̃p�[�c�̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player;						// �v���C���[
static float		camRotY =0.0f;
static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// �v���C���[�̃p�[�c�p
static XMFLOAT3		dashTbl[2];
static int			particleWait = 0;
static RASTERIZER_MODE drawMethod = CULL_MODE_BACK;

static DWORD dashcd;
static DWORD attackcd;

// �v���C���[�̊K�w�A�j���[�V�����f�[�^
// �v���C���[�̓������E�ɓ������Ă���A�j���f�[�^
static INTERPOLATION_DATA move_tbl_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(20.0f, 20.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(20.0f, 20.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },

};

static INTERPOLATION_DATA move_tbl_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },

};

static INTERPOLATION_DATA move_tbl_handLeft[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(20.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(20.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_handRight[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-20.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-20.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	move_tbl_right,
	move_tbl_left,
	move_tbl_handLeft,
	move_tbl_handRight,
};



//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model,true,&g_Player.points);
	g_Player.load = true;

	g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, 0.0f };
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };
	g_Player.time = 0.0f;
	g_Player.spd = 0.0f;			// �ړ��X�s�[�h�N���A
	g_Player.size = PLAYER_SIZE;	// �����蔻��̑傫��
	g_Player.tblNo = -1;
	g_Player.hitbox.positiona = { 0.0f, 3.0f, 0.0f };
	g_Player.hitbox.positionb = { 0.0f, PLAYER_OFFSET_Y * 2.0f -3.0f, 0.0f };
	g_Player.hitbox.radius = 3.0f;
	g_Player.use = true;

	// �����Ńv���C���[�p�̉e���쐬���Ă���
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ��
	//        ���̃����o�[�ϐ������������e��Index�ԍ�



	// �K�w�A�j���[�V�����p�̏���������
	g_Player.attachedTo = NULL;			// �{�́i�e�j�Ȃ̂�NULL������

	// �p�[�c�̏�����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		
		g_Parts[i].use = false;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, XM_PI, 0.0f);
		g_Parts[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);

		// �e�q�֌W
		g_Parts[i].attachedTo = &g_Player;		// �� �����ɐe�̃A�h���X������
	//	g_Parts[�r].attachedTo= &g_Player;		// �r��������e�͖{�́i�v���C���[�j
	//	g_Parts[��].attachedTo= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

		// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
		g_Parts[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Parts[i].tblNo = -1;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Parts[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
		g_Parts[i].load = 0;
	}
	g_Parts[0].pos = XMFLOAT3(20.0f, 15.0f, -10.0f);
	g_Parts[1].pos = XMFLOAT3(-20.0f, 15.0f, -10.0f);
	g_Parts[2].pos = XMFLOAT3(20.0f, 5.0f, 5.0f);
	g_Parts[3].pos = XMFLOAT3(-20.0f, 5.0f, 5.0f);

	//g_Parts[0].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
	//g_Parts[0].tblNo = 1;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
	//g_Parts[0].tblMax = sizeof(move_tbl_left) / sizeof(INTERPOLATION_DATA);;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
	g_Parts[0].use = true;
	g_Parts[0].load = true;

	//g_Parts[1].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
	//g_Parts[1].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
	//g_Parts[1].tblMax = sizeof(move_tbl_right) / sizeof(INTERPOLATION_DATA);;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
	g_Parts[1].use = true;
	g_Parts[1].load = true;

	g_Parts[2].use = true;
	g_Parts[2].load = true;
	g_Parts[2].rot.z = -XM_PI / 2;

	g_Parts[3].use = true;
	g_Parts[3].load = true;
	g_Parts[3].rot.z = XM_PI / 2;

	g_Parts[6].scl = XMFLOAT3(1.5f, 1.5f, 1.5f);
	g_Parts[6].use = true;
	g_Parts[6].load = true;

	//g_Parts[4].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
	//g_Parts[4].tblNo = 4;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
	//g_Parts[4].tblMax = sizeof(move_tbl_handRight) / sizeof(INTERPOLATION_DATA);;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
	//g_Parts[4].use = true;
	//g_Parts[4].load = true;

	//g_Parts[5].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
	//g_Parts[5].tblNo = 3;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
	//g_Parts[5].tblMax = sizeof(move_tbl_handLeft) / sizeof(INTERPOLATION_DATA);;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
	//g_Parts[5].use = true;
	//g_Parts[5].load = true;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	// ���f���̉������
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = false;
	}
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load)
		{
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = false;
		}
		
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();
	DWORD cdTimer = timeGetTime();
	if (g_Player.tblNo == -1) {
		// �ړ��������Ⴄ
		if (GetKeyboardPress(DIK_A))
		{	// ���ֈړ�
			g_Player.spd = VALUE_MOVE;
			g_Player.dir = XM_PI / 2;
		}
		if (GetKeyboardPress(DIK_D))
		{	// �E�ֈړ�
			g_Player.spd = VALUE_MOVE;
			g_Player.dir = -XM_PI / 2;
		}
		if (GetKeyboardPress(DIK_W))
		{	// �O�ֈړ�
			g_Player.spd = VALUE_MOVE;
			g_Player.dir = XM_PI;
		}
		if (GetKeyboardPress(DIK_S))
		{	// ���ֈړ�
			g_Player.spd = VALUE_MOVE;
			g_Player.dir = 0.0f;
		}
		if (GetKeyboardPress(DIK_LSHIFT)) 
		{
			g_Player.spd *= 2;
		}
		if (GetKeyboardTrigger(DIK_SPACE) && cdTimer - dashcd > 3000)
		{
			//DASH
			g_Player.tblNo = 1;
			g_Player.time = 0.5f;
			dashcd = cdTimer;
			dashTbl[0] = g_Player.pos;
			dashTbl[1] = XMFLOAT3(g_Player.pos.x - sinf(g_Player.rot.y) * VALUE_MOVE * 60,
				g_Player.pos.y, g_Player.pos.z - cosf(g_Player.rot.y) * VALUE_MOVE * 60);
		}
		if (IsMouseRightPressed() == FALSE) {
			camRotY = cam->rot.y;
		}
		if (GetKeyboardTrigger(DIK_Q))
		{
			if (drawMethod == CULL_MODE_BACK) {
				drawMethod = CULL_MODE_WIREFRAME;
			}
			else {
				drawMethod = CULL_MODE_BACK;
			}

		}


#ifdef _DEBUG
		if (GetKeyboardPress(DIK_R))
		{
			g_Player.pos.z = g_Player.pos.x = 0.0f;
			g_Player.rot.y = g_Player.dir = 0.0f;
			g_Player.spd = 0.0f;
		}
#endif


		//�ړ��ʂ��O�ȏ�ł���Έړ�����
		if (g_Player.spd > 0.3f)
		{
			g_Player.rot.y = g_Player.dir + camRotY;

			// ���͂̂����������փv���C���[���������Ĉړ�������
			//�v���C���[�̌����Ă��������ۑ�
			float deltaX = sinf(g_Player.rot.y) * g_Player.spd;
			float deltaZ = cosf(g_Player.rot.y) * g_Player.spd;

			//�ړ��𔽉f
			g_Player.pos.x -= deltaX;
			g_Player.pos.z -= deltaZ;

			//�ړ��ʂ��ڂɌ�����ω��ʂ̎��A
			//�p�[�e�B�N���𐶐��i�J�E���g���g�p���āA�����ʂ����炷�j
			if (g_Player.spd > VALUE_MOVE &&particleWait > 3) {
				particleWait = 0;		//�ҋ@�J�E���g���Z�b�g

				//�p�[�e�B�N���̐����p�����v�Z		
				XMFLOAT3 pos = g_Player.pos;									//�v���C���[�̍��W���N�_��
				pos.y += rand() % (int)(PLAYER_OFFSET_Y * 2) - PLAYER_OFFSET_Y;	//�v���C���[�̐g���Ȃ��Ƀ����_���ɔz�u
				pos.x += rand() % (int)(PLAYER_OFFSET_Y * 4) - PLAYER_OFFSET_Y * 2;	//�v���C���[�̐g���Ȃ��Ƀ����_���ɔz�u

				deltaX *= 10.0f;			//X���W�̈ړ���
				deltaZ *= 10.0f;			//Z���W�̈ړ���

				//�v���C���[�̑��x�ɂ���ăp�[�e�B�N���̈ړ��������ω�����
				XMFLOAT3 delta = XMFLOAT3(deltaX, 0.0f, deltaZ);

				XMFLOAT3 rot = g_Player.rot;
				rot.x += +XM_PI / 2;

				//�p�[�e�B�N���̈ړ��ʂ̕ϓ��l
				XMFLOAT3 acceleration = XMFLOAT3(0.7f, 0.0f, 0.7f);

				//�p�[�e�B�N���̐F�iRGBA 0.0f~1.0f�j
				XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);

				float pWidth = 0.25f;	//�p�[�e�B�N���̉���
				float pHeight = 0.5f;	//�p�[�e�B�N���̏c��		
				int pLife = 25;	//�p�[�e�B�N���̕\�����ԁi�t���[���P�ʁj

				SetParticle(pos, delta, acceleration, rot, color, pWidth, pHeight, false, 0, pLife);
			}
			particleWait++;
		}
	}
	else {
		g_Player.pos = GetLerp(dashTbl[0], dashTbl[1], ((1 - cosf(g_Player.time * XM_PI)) * 0.5f));
		g_Player.time += 0.1f;
		if (g_Player.time > 1.0f) {
			g_Player.tblNo = -1;
		}
	}
	// �e���v���C���[�̈ʒu�ɍ��킹��
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// ���C�L���X�g���đ����̍��������߂�
	XMFLOAT3 HitPosition;		// ��_
	XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
	bool ans = RayHitField(g_Player.pos, &HitPosition, &Normal);
	if (ans)
	{
		g_Player.pos.y = HitPosition.y + PLAYER_OFFSET_Y;
	}
	else
	{
		g_Player.pos.y = PLAYER_OFFSET_Y;
		Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}

	// �e���ˏ���
	if (IsMouseLeftTriggered())
	{
		//SetBullet(g_Player.pos, XMFLOAT3(0,100,0), XMFLOAT3(0, 0, 400));
		FireKnife(g_Player.pos, g_Player.rot);
	}

	g_Player.spd *= 0.5f;


	// �K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// �g���Ă���Ȃ珈������
		if ((g_Parts[i].use == true)&&(g_Parts[i].tblMax > 0))
		{	// ���`��Ԃ̏���
			int nowNo = (int)g_Parts[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
			int maxNo = g_Parts[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
			int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
			INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Parts[i].tblNo];	// �s���e�[�u���̃A�h���X���擾

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

			float nowTime = g_Parts[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

			Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
			Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
			Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

			// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
			XMStoreFloat3(&g_Parts[i].pos, nowPos + Pos);

			// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[i].rot, nowRot + Rot);

			// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[i].scl, nowScl + Scl);

			// frame���g�Ď��Ԍo�ߏ���������
			g_Parts[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
			if ((int)g_Parts[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			{
				g_Parts[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
			}

		}
		else {
			if (i == 6) { 
				g_Parts[i].time += 0.01f; 
				g_Parts[i].pos.y -= 0.05f * sinf(2*g_Parts[i].time);
			}
			else {
				g_Parts[i].time += 0.05f; 
				g_Parts[i].pos.y += 0.2f * sinf(g_Parts[i].time);
			}
			
			
		}

	}



	{	// �|�C���g���C�g�̃e�X�g
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = true;
		SetLightData(1, light);
	}
	
	
	{//�����蔻��
		if (true) {
			//knockBack away from leg or sword
		}
	
	}


#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Player:�� �� �� ���@Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	SetLightEnable(false);
	// �J�����O����
	SetCullingMode(drawMethod);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// ���f���`��
	DrawModel(&g_Player.model);
	XMFLOAT3* playerVerts = new XMFLOAT3[g_Player.points.VertexNum];
		for (int p = 0; p < g_Player.points.VertexNum; p++) {
			playerVerts[p] = AffineTransform(g_Player.points.VertexArray[p], XMLoadFloat4x4(&g_Player.mtxWorld));
		}


	
	// �p�[�c�̊K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].attachedTo != NULL)	// �q����������e�ƌ�������
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].attachedTo->mtxWorld));
			// ��
			// g_Player.mtxWorld���w���Ă���
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
		if (g_Parts[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);


		// ���f���`��
		DrawModel(&g_Parts[i].model);

	}
	SetLightEnable(true);


	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

XMFLOAT3 GetLerp(XMFLOAT3 initialPos, XMFLOAT3 endingPos, float percentage) {
	XMFLOAT3 result;
	XMVECTOR pos0 = XMLoadFloat3(&initialPos);
	XMVECTOR pos1 = XMLoadFloat3(&endingPos);

	pos0 += (pos1 - pos0) * percentage;
	XMStoreFloat3(&result, pos0);
	return result;
}

//337
float GetDistance3D(XMFLOAT3 pos1, XMFLOAT3 pos2) {
	return (float)sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y) + (pos1.z - pos2.z) * (pos1.z - pos2.z));
}

RASTERIZER_MODE GetRasterizerMode(void) {
	return drawMethod;
}
